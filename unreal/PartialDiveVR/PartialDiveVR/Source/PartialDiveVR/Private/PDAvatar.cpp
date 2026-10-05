#include "PDAvatar.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/OverlapResult.h"
#include "Engine/SkeletalMesh.h"
#include "Engine/World.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "IXRTrackingSystem.h"
#include "InputCoreTypes.h"
#include "MotionControllerComponent.h"
#include "PDBodyMeshComponent.h"
#include "PDBridgeJson.h"
#include "PartialDiveBridge.h"
#include "PartialDiveBridgeSubsystem.h"
#include "StereoRendering.h"
#include "TwoBoneIK.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	const FName GrabbableTag(TEXT("PDGrabbable"));

	struct FFingerChain { const TCHAR* Name; bool bThumb; };
	const FFingerChain Fingers[] = {
		{ TEXT("index"), false }, { TEXT("middle"), false }, { TEXT("ring"), false }, { TEXT("pinky"), false }, { TEXT("thumb"), true },
	};

	FName Bone(const TCHAR* Base, bool bRight) { return FName(*FString::Printf(TEXT("%s_%s"), Base, bRight ? TEXT("r") : TEXT("l"))); }
	FName Joint(const TCHAR* Finger, int32 N, bool bRight) { return FName(*FString::Printf(TEXT("%s_%02d_%s"), Finger, N, bRight ? TEXT("r") : TEXT("l"))); }

	/** Palm normal (pointing out of the palm) in component space, from hand geometry. Sign fixed per hand. */
	FVector PalmNormal(UPDBodyMeshComponent* Body, bool bRight, float Sign)
	{
		const FVector Hand = Body->GetCS(Body->BoneIndex(Bone(TEXT("hand"), bRight))).GetLocation();
		const FVector Middle = Body->GetCS(Body->BoneIndex(Joint(TEXT("middle"), 1, bRight))).GetLocation();
		const FVector Index = Body->GetCS(Body->BoneIndex(Joint(TEXT("index"), 1, bRight))).GetLocation();
		const FVector Pinky = Body->GetCS(Body->BoneIndex(Joint(TEXT("pinky"), 1, bRight))).GetLocation();
		return Sign * FVector::CrossProduct((Middle - Hand).GetSafeNormal(), (Index - Pinky).GetSafeNormal()).GetSafeNormal();
	}
}

// --------------------------------------------------------------------------- construction

APDAvatar::APDAvatar(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	bUseControllerRotationYaw = false;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;
	AutoPossessPlayer = EAutoReceiveInput::Disabled;

	GetCapsuleComponent()->InitCapsuleSize(34.f, 92.f);

	UCharacterMovementComponent* Move = GetCharacterMovement();
	Move->bOrientRotationToMovement = false;
	Move->MaxWalkSpeed = MaxWalkSpeedCm;
	Move->BrakingDecelerationWalking = 800.f;
	Move->MaxAcceleration = 600.f;

	// Driver mesh: animated, never rendered.
	static ConstructorHelpers::FObjectFinder<USkeletalMesh> MannyMesh(TEXT("/Game/Characters/Mannequins/Meshes/SKM_Manny_Simple.SKM_Manny_Simple"));
	static ConstructorHelpers::FClassFinder<UAnimInstance> UnarmedAnim(TEXT("/Game/Characters/Mannequins/Anims/Unarmed/ABP_Unarmed"));
	USkeletalMeshComponent* Driver = GetMesh();
	if (MannyMesh.Succeeded()) Driver->SetSkeletalMesh(MannyMesh.Object);
	if (UnarmedAnim.Succeeded()) Driver->SetAnimInstanceClass(UnarmedAnim.Class);
	Driver->SetRelativeLocationAndRotation(FVector(0, 0, -92.f), FRotator(0, -90.f, 0));
	Driver->SetHiddenInGame(true);
	Driver->SetCastShadow(false);
	Driver->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::AlwaysTickPoseAndRefreshBones;
	Driver->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Visible body.
	Body = CreateDefaultSubobject<UPDBodyMeshComponent>(TEXT("Body"));
	Body->SetupAttachment(GetCapsuleComponent());
	Body->SetRelativeLocationAndRotation(Driver->GetRelativeLocation(), Driver->GetRelativeRotation());
	if (MannyMesh.Succeeded()) Body->SetSkinnedAssetAndUpdate(MannyMesh.Object);
	Body->Driver = Driver;
	Body->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Body->bCastHiddenShadow = false;

	// VR rig: VROrigin is moved by Recenter() so the HMD-driven camera lands at the avatar's eyes.
	VROrigin = CreateDefaultSubobject<USceneComponent>(TEXT("VROrigin"));
	VROrigin->SetupAttachment(GetCapsuleComponent());

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VROrigin);
	Camera->bLockToHmd = true;
	Camera->bUsePawnControlRotation = false;

	LeftController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftController"));
	LeftController->SetupAttachment(VROrigin);
	LeftController->SetTrackingMotionSource(FName(TEXT("Left")));
	RightController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightController"));
	RightController->SetupAttachment(VROrigin);
	RightController->SetTrackingMotionSource(FName(TEXT("Right")));

	// Debug third-person view (pdive.view front|back) to check embodiment/eye point from outside.
	DebugBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("DebugBoom"));
	DebugBoom->SetupAttachment(GetCapsuleComponent());
	DebugBoom->TargetArmLength = 260.f;
	DebugBoom->bDoCollisionTest = false;
	DebugBoom->SetRelativeLocation(FVector(0, 0, 40.f));
	DebugCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("DebugCamera"));
	DebugCamera->SetupAttachment(DebugBoom);
	DebugCamera->bLockToHmd = false;
	DebugCamera->SetAutoActivate(false);
}

UPartialDiveBridgeSubsystem* APDAvatar::Bridge() const
{
	const UGameInstance* GI = GetGameInstance();
	return GI ? GI->GetSubsystem<UPartialDiveBridgeSubsystem>() : nullptr;
}

bool APDAvatar::IsHmdActive() const
{
	return GEngine && GEngine->XRSystem.IsValid() && GEngine->StereoRenderingDevice.IsValid()
		&& GEngine->StereoRenderingDevice->IsStereoEnabled();
}

void APDAvatar::BeginPlay()
{
	Super::BeginPlay();
	GetCharacterMovement()->MaxWalkSpeed = MaxWalkSpeedCm;
	Body->AddTickPrerequisiteComponent(GetMesh());

	static const TCHAR* Required[] = { TEXT("head"), TEXT("neck_01"), TEXT("hand_l"), TEXT("hand_r"), TEXT("lowerarm_l"),
		TEXT("lowerarm_r"), TEXT("upperarm_l"), TEXT("upperarm_r"), TEXT("index_01_l"), TEXT("pinky_01_r"), TEXT("thumb_03_r") };
	for (const TCHAR* Name : Required)
	{
		if (Body->BoneIndex(Name) == INDEX_NONE)
		{
			UE_LOG(LogPartialDive, Error, TEXT("Avatar: bone '%s' missing from body mesh; procedural pose disabled for it"), Name);
		}
	}

	ComputeEyePoint();
	BuildTouchZones();
	SetViewMode(ViewMode);

	if (UPartialDiveBridgeSubsystem* B = Bridge())
	{
		B->OnBlockStart.AddDynamic(this, &APDAvatar::HandleBlockStart);
	}
}

void APDAvatar::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	PossessedAt = GetWorld()->GetTimeSeconds();
	bRecentered = false;
}

void APDAvatar::ComputeEyePoint()
{
	// From the bind pose, so walking animation never bobs the camera (comfort).
	const FTransform HeadCS = Body->GetRefPoseCS(TEXT("head"));
	const FVector HeadActor = Body->GetRelativeTransform().TransformPosition(HeadCS.GetLocation());
	EyeLocal = HeadActor + EyeOffsetFromHead;
}

// --------------------------------------------------------------------------- view

void APDAvatar::Recenter()
{
	if (IsHmdActive())
	{
		// Camera's relative transform == HMD pose in tracking space.
		const FTransform Hmd = Camera->GetRelativeTransform();
		const FRotator HmdRot = Hmd.Rotator();
		const FQuat Neutral = FRotator(HmdRot.Pitch, HmdRot.Yaw, 0.f).Quaternion(); // ignore roll
		const FQuat OriginRot = Neutral.Inverse();
		VROrigin->SetRelativeLocationAndRotation(EyeLocal - OriginRot.RotateVector(Hmd.GetLocation()), OriginRot);
		UE_LOG(LogPartialDive, Log, TEXT("Avatar recentered: physical pitch %.0f deg, yaw %.0f deg -> avatar straight ahead"), HmdRot.Pitch, HmdRot.Yaw);
	}
	else
	{
		VROrigin->SetRelativeLocationAndRotation(EyeLocal, FQuat::Identity);
		Camera->SetRelativeLocationAndRotation(FVector::ZeroVector, FRotator(DesktopPitch, 0, 0));
	}
	bRecentered = true;
	if (UPartialDiveBridgeSubsystem* B = Bridge())
	{
		B->PushGameMarker(TEXT("recenter"), {});
	}
}

void APDAvatar::SetViewMode(EPDViewMode NewMode)
{
	ViewMode = NewMode;
	const bool bFirst = NewMode == EPDViewMode::FirstPerson;
	Camera->SetActive(bFirst);
	DebugCamera->SetActive(!bFirst);
	const bool bFront = NewMode == EPDViewMode::ThirdPersonFront || NewMode == EPDViewMode::HandCloseUp;
	DebugBoom->SetRelativeRotation(FRotator(NewMode == EPDViewMode::HandCloseUp ? 0.f : -12.f, bFront ? 180.f : 0.f, 0.f));
	DebugBoom->TargetArmLength = NewMode == EPDViewMode::HandCloseUp ? 55.f : bFront ? 170.f : 260.f;
	// Hand close-up: boom pivot beside the right hip (actor right = +Y), roughly at wrist height.
	DebugBoom->SetRelativeLocation(NewMode == EPDViewMode::HandCloseUp ? FVector(10.f, 30.f, -10.f) : FVector(0.f, 0.f, 40.f));
}

// --------------------------------------------------------------------------- per-frame

void APDAvatar::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (!bRecentered && PossessedAt >= 0.0)
	{
		const bool bHmd = IsHmdActive();
		const bool bTracking = bHmd && !Camera->GetRelativeLocation().IsNearlyZero();
		if (!bHmd || (bTracking && GetWorld()->GetTimeSeconds() - PossessedAt >= AutoRecenterDelay))
		{
			Recenter();
		}
	}

	float Walk = 0.f, Strafe = 0.f, TurnRate = 0.f, GrabL = 0.f, GrabR = 0.f;
	ReadInputs(DeltaSeconds, Walk, Strafe, TurnRate, GrabL, GrabR);

	if (!FMath::IsNearlyZero(Walk)) AddMovementInput(GetActorForwardVector(), FMath::Clamp(Walk, -1.f, 1.f));
	if (!FMath::IsNearlyZero(Strafe)) AddMovementInput(GetActorRightVector(), FMath::Clamp(Strafe, -1.f, 1.f));
	if (!FMath::IsNearlyZero(TurnRate)) AddActorWorldRotation(FRotator(0.f, TurnRate * DeltaSeconds, 0.f));

	const float K = FMath::Clamp(HandResponse * DeltaSeconds, 0.f, 1.f);
	HandClosureL += (FMath::Clamp(GrabL, 0.f, 1.f) - HandClosureL) * K;
	HandClosureR += (FMath::Clamp(GrabR, 0.f, 1.f) - HandClosureR) * K;
	UpdateGrabState(false, GrabL);
	UpdateGrabState(true, GrabR);

	if (ViewMode != EPDViewMode::FirstPerson)
	{
		DrawDebugSphere(GetWorld(), Camera->GetComponentLocation(), 3.f, 8, FColor::Cyan);
	}
}

void APDAvatar::ReadInputs(float DeltaSeconds, float& OutWalk, float& OutStrafe, float& OutTurnRate, float& OutGrabL, float& OutGrabR)
{
	APlayerController* PC = Cast<APlayerController>(GetController());
	UPartialDiveBridgeSubsystem* B = Bridge();
	const bool bIntent = B && B->GetInputSource() == EPDInputSource::Intent;
	SnapTurnCooldown = FMath::Max(0.f, SnapTurnCooldown - DeltaSeconds);

	if (bIntent)
	{
		const FPDIntentFrame F = B->GetIntent(); // zeros when stale
		OutWalk = F.WalkForward;
		OutTurnRate = (F.TurnRight - F.TurnLeft) * IntentTurnRateDeg;
		OutGrabL = F.GrabLeft;
		OutGrabR = F.GrabRight;
	}

	if (PC)
	{
		auto Down = [PC](const FKey& Key) { return PC->IsInputKeyDown(Key) ? 1.f : 0.f; };
		auto Axis = [PC](const FKey& Key) { return PC->GetInputAnalogKeyState(Key); };

		// Always available: recenter, view cycle, input-source toggle (dev/session control, not gameplay).
		if (PC->WasInputKeyJustPressed(EKeys::R) ||
			(PC->IsInputKeyDown(EKeys::OculusTouch_Left_Thumbstick_Click) && PC->WasInputKeyJustPressed(EKeys::OculusTouch_Right_Thumbstick_Click)))
		{
			Recenter();
		}
		if (PC->WasInputKeyJustPressed(EKeys::V))
		{
			SetViewMode(static_cast<EPDViewMode>((static_cast<uint8>(ViewMode) + 1) % 4));
		}
		if (PC->WasInputKeyJustPressed(EKeys::I) && B)
		{
			B->SetInputSource(bIntent ? EPDInputSource::Controller : EPDInputSource::Intent);
		}

		if (!bIntent)
		{
			// Conventional baseline: Touch controllers + keyboard.
			OutWalk = Axis(EKeys::OculusTouch_Left_Thumbstick_Y) + Down(EKeys::W) - Down(EKeys::S);
			OutStrafe = Axis(EKeys::OculusTouch_Left_Thumbstick_X) + Down(EKeys::D) - Down(EKeys::A);
			OutTurnRate = (Down(EKeys::E) - Down(EKeys::Q)) * IntentTurnRateDeg;
			const float Snap = Axis(EKeys::OculusTouch_Right_Thumbstick_X);
			if (FMath::Abs(Snap) > 0.7f && SnapTurnCooldown <= 0.f)
			{
				AddActorWorldRotation(FRotator(0.f, FMath::Sign(Snap) * SnapTurnDeg, 0.f));
				SnapTurnCooldown = 0.35f;
			}
			OutGrabL = FMath::Max(Axis(EKeys::OculusTouch_Left_Grip_Axis), Down(EKeys::F));
			OutGrabR = FMath::Max(Axis(EKeys::OculusTouch_Right_Grip_Axis), Down(EKeys::G));
		}

		if (!IsHmdActive())
		{
			float Dx = 0.f, Dy = 0.f;
			PC->GetInputMouseDelta(Dx, Dy);
			AddActorWorldRotation(FRotator(0.f, Dx * 0.6f, 0.f));
			DesktopPitch = FMath::Clamp(DesktopPitch + Dy * 0.6f, -80.f, 80.f);
			Camera->SetRelativeRotation(FRotator(DesktopPitch, 0.f, 0.f));
		}
	}

	if (DebugGrabL >= 0.f) OutGrabL = DebugGrabL;
	if (DebugGrabR >= 0.f) OutGrabR = DebugGrabR;
}

// --------------------------------------------------------------------------- grabbing

void APDAvatar::UpdateGrabState(bool bRight, float Value)
{
	bool& bGrabbing = bRight ? bGrabbingR : bGrabbingL;
	if (!bGrabbing && Value >= GrabOnThreshold)
	{
		bGrabbing = true;
		TryGrab(bRight);
	}
	else if (bGrabbing && Value <= GrabOffThreshold)
	{
		bGrabbing = false;
		Release(bRight);
	}
}

void APDAvatar::TryGrab(bool bRight)
{
	TObjectPtr<AActor>& Held = bRight ? HeldR : HeldL;
	if (Held)
	{
		return;
	}
	const FName HandBone = Bone(TEXT("hand"), bRight);
	const FVector Palm = (Body->GetSocketLocation(HandBone) + Body->GetSocketLocation(Joint(TEXT("middle"), 1, bRight))) * 0.5;

	TArray<FOverlapResult> Overlaps;
	FCollisionObjectQueryParams Objects;
	Objects.AddObjectTypesToQuery(ECC_PhysicsBody);
	Objects.AddObjectTypesToQuery(ECC_WorldDynamic);
	FCollisionQueryParams Params(SCENE_QUERY_STAT(PDGrab), false, this);
	GetWorld()->OverlapMultiByObjectType(Overlaps, Palm, FQuat::Identity, Objects, FCollisionShape::MakeSphere(GrabRadiusCm), Params);

	AActor* Best = nullptr;
	double BestDist = TNumericLimits<double>::Max();
	for (const FOverlapResult& O : Overlaps)
	{
		AActor* A = O.GetActor();
		if (A && A->ActorHasTag(GrabbableTag) && A != HeldL && A != HeldR)
		{
			const double D = FVector::DistSquared(A->GetActorLocation(), Palm);
			if (D < BestDist) { BestDist = D; Best = A; }
		}
	}
	if (!Best)
	{
		return;
	}
	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Best->GetRootComponent()))
	{
		Root->SetSimulatePhysics(false);
	}
	Best->AttachToComponent(Body, FAttachmentTransformRules::KeepWorldTransform, HandBone);
	Held = Best;

	if (UPartialDiveBridgeSubsystem* B = Bridge())
	{
		B->EmitHaptic(EPDHapticKind::Contact, bRight ? EPDHapticZone::RightHand : EPDHapticZone::LeftHand, 0.5f, 80, FString(), TEXT("grab:") + Best->GetName());
		B->PushGameMarker(TEXT("grab"), { { TEXT("hand"), bRight ? TEXT("right") : TEXT("left") }, { TEXT("actor"), Best->GetName() } });
	}
}

void APDAvatar::Release(bool bRight)
{
	TObjectPtr<AActor>& Held = bRight ? HeldR : HeldL;
	if (!Held)
	{
		return;
	}
	Held->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	if (UPrimitiveComponent* Root = Cast<UPrimitiveComponent>(Held->GetRootComponent()))
	{
		Root->SetSimulatePhysics(true);
	}
	if (UPartialDiveBridgeSubsystem* B = Bridge())
	{
		B->PushGameMarker(TEXT("release"), { { TEXT("hand"), bRight ? TEXT("right") : TEXT("left") }, { TEXT("actor"), Held->GetName() } });
	}
	Held = nullptr;
}

// --------------------------------------------------------------------------- touch -> haptics

void APDAvatar::BuildTouchZones()
{
	struct FDef { EPDHapticZone Zone; const TCHAR* From; const TCHAR* To; float Radius; };
	static const FDef Defs[] = {
		{ EPDHapticZone::LeftHand, TEXT("hand_l"), TEXT("middle_01_l"), 5.f },
		{ EPDHapticZone::RightHand, TEXT("hand_r"), TEXT("middle_01_r"), 5.f },
		{ EPDHapticZone::LeftForearm, TEXT("lowerarm_l"), TEXT("hand_l"), 5.f },
		{ EPDHapticZone::RightForearm, TEXT("lowerarm_r"), TEXT("hand_r"), 5.f },
		{ EPDHapticZone::LeftUpperArm, TEXT("upperarm_l"), TEXT("lowerarm_l"), 6.f },
		{ EPDHapticZone::RightUpperArm, TEXT("upperarm_r"), TEXT("lowerarm_r"), 6.f },
		{ EPDHapticZone::LeftShoulder, TEXT("clavicle_l"), TEXT("upperarm_l"), 6.f },
		{ EPDHapticZone::RightShoulder, TEXT("clavicle_r"), TEXT("upperarm_r"), 6.f },
		{ EPDHapticZone::Chest, TEXT("spine_04"), TEXT("neck_01"), 13.f },
		{ EPDHapticZone::Core, TEXT("pelvis"), TEXT("spine_03"), 13.f },
		{ EPDHapticZone::LeftThigh, TEXT("thigh_l"), TEXT("calf_l"), 8.f },
		{ EPDHapticZone::RightThigh, TEXT("thigh_r"), TEXT("calf_r"), 8.f },
		{ EPDHapticZone::LeftFoot, TEXT("foot_l"), TEXT("ball_l"), 5.f },
		{ EPDHapticZone::RightFoot, TEXT("foot_r"), TEXT("ball_r"), 5.f },
	};
	for (const FDef& D : Defs)
	{
		if (Body->BoneIndex(D.From) == INDEX_NONE || Body->BoneIndex(D.To) == INDEX_NONE)
		{
			continue;
		}
		const FTransform FromCS = Body->GetRefPoseCS(D.From);
		const FVector Local = FromCS.InverseTransformPosition(Body->GetRefPoseCS(D.To).GetLocation());
		const float Length = Local.Size();

		UCapsuleComponent* Cap = NewObject<UCapsuleComponent>(this, *FString::Printf(TEXT("Touch_%s"), *PDBridgeJson::ZoneName(D.Zone)));
		Cap->SetupAttachment(Body, D.From);
		Cap->SetCapsuleSize(D.Radius, Length * 0.5f + D.Radius);
		Cap->SetRelativeLocation(Local * 0.5);
		Cap->SetRelativeRotation(FQuat::FindBetweenNormals(FVector::UpVector, Local.GetSafeNormal()));
		Cap->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		Cap->SetCollisionObjectType(ECC_WorldDynamic);
		Cap->SetCollisionResponseToAllChannels(ECR_Ignore);
		Cap->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
		Cap->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Overlap);
		Cap->SetGenerateOverlapEvents(true);
		Cap->SetHiddenInGame(true);
		Cap->RegisterComponent();
		Cap->OnComponentBeginOverlap.AddDynamic(this, &APDAvatar::HandleTouchBegin);

		FPDTouchZone Z;
		Z.Capsule = Cap;
		Z.Zone = D.Zone;
		TouchZones.Add(Z);
	}
}

void APDAvatar::HandleTouchBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == HeldL || OtherActor == HeldR)
	{
		return;
	}
	const double Now = GetWorld()->GetTimeSeconds();
	for (FPDTouchZone& Z : TouchZones)
	{
		if (Z.Capsule != OverlappedComponent)
		{
			continue;
		}
		if (Now - Z.LastFired < TouchCooldownSeconds)
		{
			return;
		}
		Z.LastFired = Now;
		const float Speed = OtherComp ? (OtherComp->GetComponentVelocity() - GetVelocity()).Size() : 0.f;
		const float Strength = FMath::GetMappedRangeValueClamped(FVector2f(0.f, 200.f), FVector2f(0.35f, 1.f), Speed);
		if (UPartialDiveBridgeSubsystem* B = Bridge())
		{
			B->EmitHaptic(EPDHapticKind::Contact, Z.Zone, Strength, TouchDurationMs, FString(), TEXT("touch:") + OtherActor->GetName());
		}
		return;
	}
}

// --------------------------------------------------------------------------- procedural pose

void APDAvatar::ApplyProceduralPose(float DeltaSeconds)
{
	ApplyHeadLook();
	for (const bool bRight : { false, true })
	{
		UMotionControllerComponent* MC = bRight ? RightController : LeftController;
		const bool bTracked = MC && MC->IsTracked() && IsHmdActive();
		if (ArmSource == EPDArmSource::Tracked || (ArmSource == EPDArmSource::Auto && bTracked))
		{
			ApplyArmIK(bRight);
		}
		ApplyFingerCurl(bRight, bRight ? HandClosureR : HandClosureL);
	}
}

void APDAvatar::ApplyHeadLook()
{
	const int32 Neck = Body->BoneIndex(TEXT("neck_01"));
	const int32 Head = Body->BoneIndex(TEXT("head"));
	if (Neck == INDEX_NONE || Head == INDEX_NONE)
	{
		return;
	}
	const FQuat ActorQ = GetActorQuat();
	const FRotator Rel = (ActorQ.Inverse() * Camera->GetComponentQuat()).Rotator();
	const FQuat LookActor = FRotator(FMath::Clamp(Rel.Pitch, -HeadPitchLimitDeg, HeadPitchLimitDeg),
		FMath::Clamp(Rel.Yaw, -HeadYawLimitDeg, HeadYawLimitDeg), 0.f).Quaternion();
	// actor-space rotation -> component-space delta
	const FQuat CompInActor = Body->GetRelativeTransform().GetRotation();
	const FQuat Delta = CompInActor.Inverse() * LookActor * CompInActor;
	const FQuat Half = FQuat::Slerp(FQuat::Identity, Delta, 0.5f);
	Body->RotateCS(Neck, Half);
	Body->RotateCS(Head, Half);
}

void APDAvatar::ApplyArmIK(bool bRight)
{
	const int32 Upper = Body->BoneIndex(Bone(TEXT("upperarm"), bRight));
	const int32 Lower = Body->BoneIndex(Bone(TEXT("lowerarm"), bRight));
	const int32 Hand = Body->BoneIndex(Bone(TEXT("hand"), bRight));
	const int32 Middle = Body->BoneIndex(Joint(TEXT("middle"), 1, bRight));
	const int32 Spine = Body->BoneIndex(TEXT("spine_05"));
	UMotionControllerComponent* MC = bRight ? RightController : LeftController;
	if (Upper == INDEX_NONE || Lower == INDEX_NONE || Hand == INDEX_NONE || Middle == INDEX_NONE || !MC)
	{
		return;
	}
	const FTransform CompToWorld = Body->GetComponentTransform();

	// Desired hand orientation: fingers along the controller's forward axis, palm facing the body midline.
	// The hand's own (finger, palm) basis comes from the bind pose, so no bone-axis conventions are assumed.
	const FTransform HandRef = Body->GetRefPoseCS(Bone(TEXT("hand"), bRight));
	const FVector FingerRefCS = (Body->GetRefPoseCS(Joint(TEXT("middle"), 1, bRight)).GetLocation() - HandRef.GetLocation()).GetSafeNormal();
	const FVector IndexRef = Body->GetRefPoseCS(Joint(TEXT("index"), 1, bRight)).GetLocation();
	const FVector PinkyRef = Body->GetRefPoseCS(Joint(TEXT("pinky"), 1, bRight)).GetLocation();
	FVector PalmRefCS = FVector::CrossProduct(FingerRefCS, (IndexRef - PinkyRef).GetSafeNormal()).GetSafeNormal();
	if (PalmRefCS.Z > 0.f) PalmRefCS = -PalmRefCS; // A-pose: palms face down
	const FQuat LocalBasis = HandRef.GetRotation().Inverse() * FRotationMatrix::MakeFromXY(FingerRefCS, PalmRefCS).ToQuat();
	const FTransform Ctrl = MC->GetComponentTransform();
	const FVector PalmTargetWorld = Ctrl.GetRotation().GetRightVector() * (bRight ? -1.0 : 1.0);
	const FQuat HandWorld = FRotationMatrix::MakeFromXY(Ctrl.GetRotation().GetForwardVector(), PalmTargetWorld).ToQuat() * LocalBasis.Inverse();
	const FQuat HandCS = CompToWorld.GetRotation().Inverse() * HandWorld;

	// Wrist sits a little behind the controller's grip point.
	const FVector WristWorld = Ctrl.GetLocation() - Ctrl.GetRotation().GetForwardVector() * 7.0 - PalmTargetWorld * 2.0;
	const FVector Effector = CompToWorld.InverseTransformPosition(WristWorld);

	const FVector Shoulder = Body->GetCS(Upper).GetLocation();
	const FVector Elbow = Body->GetCS(Lower).GetLocation();
	const FVector Wrist = Body->GetCS(Hand).GetLocation();
	const FVector Outward = Spine != INDEX_NONE ? (Shoulder - Body->GetCS(Spine).GetLocation()).GetSafeNormal2D() : FVector::ZeroVector;
	const FVector Pole = Elbow + FVector(0, 0, -40.0) + Outward * 25.0;

	FVector NewElbow, NewWrist;
	AnimationCore::SolveTwoBoneIK(Shoulder, Elbow, Wrist, Pole, Effector, NewElbow, NewWrist, false, 1.0, 1.0);

	Body->RotateCS(Upper, FQuat::FindBetweenNormals((Elbow - Shoulder).GetSafeNormal(), (NewElbow - Shoulder).GetSafeNormal()));
	const FVector Elbow2 = Body->GetCS(Lower).GetLocation();
	const FVector Wrist2 = Body->GetCS(Hand).GetLocation();
	Body->RotateCS(Lower, FQuat::FindBetweenNormals((Wrist2 - Elbow2).GetSafeNormal(), (NewWrist - Elbow2).GetSafeNormal()));
	Body->SetCSRotation(Hand, HandCS);
}

void APDAvatar::ApplyFingerCurl(bool bRight, float Alpha)
{
	if (Alpha <= KINDA_SMALL_NUMBER || Body->BoneIndex(Bone(TEXT("hand"), bRight)) == INDEX_NONE)
	{
		return;
	}
	// Palm normal sign: fixed so that in the bind pose the palm faces down (A-pose).
	FVector Palm = PalmNormal(Body, bRight, 1.f);
	{
		const FVector HandRef = Body->GetRefPoseCS(Bone(TEXT("hand"), bRight)).GetLocation();
		const FVector MidRef = Body->GetRefPoseCS(Joint(TEXT("middle"), 1, bRight)).GetLocation();
		const FVector IdxRef = Body->GetRefPoseCS(Joint(TEXT("index"), 1, bRight)).GetLocation();
		const FVector PinRef = Body->GetRefPoseCS(Joint(TEXT("pinky"), 1, bRight)).GetLocation();
		const FVector RefPalm = FVector::CrossProduct((MidRef - HandRef).GetSafeNormal(), (IdxRef - PinRef).GetSafeNormal());
		if (RefPalm.Z > 0.f) Palm = -Palm;
	}

	for (const FFingerChain& Finger : Fingers) // thumb is last, so it wraps over already-curled fingers
	{
		const FVector Angles = Finger.bThumb ? ThumbCurlDeg : FingerCurlDeg;
		for (int32 N = 1; N <= 3; ++N)
		{
			const int32 B = Body->BoneIndex(Joint(Finger.Name, N, bRight));
			if (B == INDEX_NONE)
			{
				break;
			}
			const int32 Child = Body->BoneIndex(Joint(Finger.Name, N + 1, bRight));
			const int32 Parent = Body->GetSkinnedAsset()->GetRefSkeleton().GetParentIndex(B);
			const FVector From = Body->GetCS(B).GetLocation();
			const FVector Dir = Child != INDEX_NONE
				? (Body->GetCS(Child).GetLocation() - From).GetSafeNormal()
				: (From - Body->GetCS(Parent).GetLocation()).GetSafeNormal();

			FVector Toward = Palm; // fingers: fold toward the palm
			double MaxDeg = 180.0;
			if (Finger.bThumb)
			{
				// Thumb: wrap toward the (curled) index/middle middle knuckles, like a fist.
				const int32 I2 = Body->BoneIndex(Joint(TEXT("index"), 2, bRight));
				const int32 M2 = Body->BoneIndex(Joint(TEXT("middle"), 2, bRight));
				if (I2 == INDEX_NONE || M2 == INDEX_NONE)
				{
					break;
				}
				Toward = ((Body->GetCS(I2).GetLocation() + Body->GetCS(M2).GetLocation()) * 0.5 - From).GetSafeNormal();
				MaxDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(FVector::DotProduct(Dir, Toward), -1.0, 1.0)));
			}
			// Rotating Dir about (Dir x Toward) by +angle moves the tip toward Toward.
			const FVector Axis = FVector::CrossProduct(Dir, Toward).GetSafeNormal();
			if (Axis.IsNearlyZero())
			{
				continue;
			}
			const double Deg = FMath::Min<double>(Angles[N - 1] * Alpha, MaxDeg);
			Body->RotateCS(B, FQuat(Axis, FMath::DegreesToRadians(Deg)));
		}
	}
}

void APDAvatar::LogHandStats(bool bRight) const
{
	const int32 Hand = Body->BoneIndex(Bone(TEXT("hand"), bRight));
	const int32 Tip = Body->BoneIndex(Joint(TEXT("index"), 3, bRight));
	const int32 Mid = Body->BoneIndex(Joint(TEXT("middle"), 1, bRight));
	if (Hand == INDEX_NONE || Tip == INDEX_NONE || Mid == INDEX_NONE)
	{
		return;
	}
	const TArray<FTransform>& Anim = GetMesh()->GetComponentSpaceTransforms(); // relaxed, animated
	const TArray<FTransform>& Shown = Body->GetComponentSpaceTransforms();      // after procedural edits
	FVector Palm = PalmNormal(Body, bRight, 1.f);
	{
		const FVector HandRef = Body->GetRefPoseCS(Bone(TEXT("hand"), bRight)).GetLocation();
		const FVector RefPalm = FVector::CrossProduct((Body->GetRefPoseCS(Joint(TEXT("middle"), 1, bRight)).GetLocation() - HandRef).GetSafeNormal(),
			(Body->GetRefPoseCS(Joint(TEXT("index"), 1, bRight)).GetLocation() - Body->GetRefPoseCS(Joint(TEXT("pinky"), 1, bRight)).GetLocation()).GetSafeNormal());
		if (RefPalm.Z > 0.f) Palm = -Palm;
	}
	auto Stats = [&](const TArray<FTransform>& CS, const TCHAR* Label)
	{
		const FVector H = CS[Hand].GetLocation(), K = CS[Mid].GetLocation(), T = CS[Tip].GetLocation();
		UE_LOG(LogPartialDive, Display, TEXT("HandStats %s %s: tip-wrist %.1f cm, tip offset along palm normal %.1f cm (positive = palm side)"),
			bRight ? TEXT("R") : TEXT("L"), Label, FVector::Dist(T, H), FVector::DotProduct(T - K, Palm));
	};
	Stats(Anim, TEXT("relaxed"));
	Stats(Shown, TEXT("shown  "));
}

// --------------------------------------------------------------------------- experiment blocks

void APDAvatar::HandleBlockStart(const FPDBlockInfo& Block)
{
	UPartialDiveBridgeSubsystem* B = Bridge();
	if (!B)
	{
		return;
	}
	// Condition params may switch the input source: input/locomotion = emg|eeg|fusion|intent vs keyboard|joystick|controller.
	for (const TCHAR* Key : { TEXT("input"), TEXT("locomotion") })
	{
		const FString V = B->GetBlockParamString(Key, FString()).ToLower();
		if (V == TEXT("emg") || V == TEXT("eeg") || V == TEXT("fusion") || V == TEXT("intent"))
		{
			B->SetInputSource(EPDInputSource::Intent);
		}
		else if (V == TEXT("keyboard") || V == TEXT("joystick") || V == TEXT("controller") || V == TEXT("button"))
		{
			B->SetInputSource(EPDInputSource::Controller);
		}
	}
}
