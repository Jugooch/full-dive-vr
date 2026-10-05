#include "PDLabProps.h"

#include "Camera/PlayerCameraManager.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/GameViewportClient.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	UStaticMesh* LoadShape(const TCHAR* Shape)
	{
		return LoadObject<UStaticMesh>(nullptr, *FString::Printf(TEXT("/Engine/BasicShapes/%s.%s"), Shape, Shape));
	}
	UMaterialInterface* BasicMaterial()
	{
		return LoadObject<UMaterialInterface>(nullptr, TEXT("/Engine/BasicShapes/BasicShapeMaterial.BasicShapeMaterial"));
	}
}

APDProp::APDProp()
{
	Root = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Root"));
	RootComponent = Root;
}

UStaticMeshComponent* APDProp::AddShape(const TCHAR* Shape, const FVector& Location, const FVector& Scale, const FLinearColor& Tint, USceneComponent* Parent)
{
	UStaticMeshComponent* C = Parent ? NewObject<UStaticMeshComponent>(this) : Root.Get();
	C->SetStaticMesh(LoadShape(Shape));
	if (Parent)
	{
		C->SetupAttachment(Parent);
		C->CreationMethod = EComponentCreationMethod::UserConstructionScript;
		C->RegisterComponent();
	}
	if (Parent)
	{
		// Children use absolute scale and unscaled offsets (cm from the root origin), so a scaled root
		// (e.g. the sword handle) doesn't distort them.
		C->SetUsingAbsoluteScale(true);
		C->SetRelativeLocation(Location / Parent->GetRelativeScale3D());
	}
	// (The root stays at the actor origin; place the actor itself where the root should be.)
	C->SetRelativeScale3D(Scale);
	if (UMaterialInstanceDynamic* MID = UMaterialInstanceDynamic::Create(BasicMaterial(), this))
	{
		MID->SetVectorParameterValue(TEXT("Color"), Tint);
		C->SetMaterial(0, MID);
	}
	return C;
}

void APDProp::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	Tags.Remove(TEXT("PDGrabbable"));
	Root->SetSimulatePhysics(false);

	switch (Kind)
	{
	case EPDPropKind::Cube:
		AddShape(TEXT("Cube"), FVector::ZeroVector, FVector(0.08), Color, nullptr);
		break;
	case EPDPropKind::Sphere:
		AddShape(TEXT("Sphere"), FVector::ZeroVector, FVector(0.09), Color, nullptr);
		break;
	case EPDPropKind::Sword:
	{
		// Handle is the physics root; guard and blade weld to it.
		AddShape(TEXT("Cylinder"), FVector::ZeroVector, FVector(0.035, 0.035, 0.18), FLinearColor(0.25f, 0.15f, 0.08f), nullptr);
		AddShape(TEXT("Cube"), FVector(0, 0, 10.5), FVector(0.03, 0.22, 0.03), FLinearColor(0.6f, 0.55f, 0.3f), Root);
		AddShape(TEXT("Cube"), FVector(0, 0, 50.0), FVector(0.012, 0.06, 0.75), FLinearColor(0.85f, 0.88f, 0.92f), Root);
		break;
	}
	case EPDPropKind::Table:
	{
		// Root = tabletop (place the actor at z = 75 cm); legs hang below it.
		AddShape(TEXT("Cube"), FVector::ZeroVector, FVector(0.8, 1.4, 0.05), Color, nullptr);
		for (const FVector2D Leg : { FVector2D(35, 65), FVector2D(-35, 65), FVector2D(35, -65), FVector2D(-35, -65) })
		{
			AddShape(TEXT("Cube"), FVector(Leg.X, Leg.Y, -37.5), FVector(0.05, 0.05, 0.72), Color * 0.6f, Root);
		}
		break;
	}
	case EPDPropKind::TargetDummy:
	{
		// Root = 160 cm body (place the actor at z = 80 cm); head on top.
		AddShape(TEXT("Cylinder"), FVector::ZeroVector, FVector(0.4, 0.4, 1.6), Color, nullptr);
		AddShape(TEXT("Sphere"), FVector(0, 0, 105), FVector(0.45), Color, Root);
		break;
	}
	}

	const bool bGrabbable = Kind == EPDPropKind::Cube || Kind == EPDPropKind::Sphere || Kind == EPDPropKind::Sword;
	Root->SetCollisionProfileName(bGrabbable ? TEXT("PhysicsActor") : TEXT("BlockAllDynamic"));
	Root->SetGenerateOverlapEvents(true);
	if (bGrabbable)
	{
		Tags.AddUnique(TEXT("PDGrabbable"));
		Root->SetSimulatePhysics(true);
	}
}

APDMirror::APDMirror()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PostUpdateWork; // after the player camera has updated this frame

	static ConstructorHelpers::FObjectFinder<UStaticMesh> Plane(TEXT("/Engine/BasicShapes/Plane.Plane"));
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	// 120 x 200 cm surface standing on the floor; plane normal (+Z) rotated to face the actor's +X.
	Surface = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Surface"));
	Surface->SetupAttachment(RootComponent);
	Surface->SetStaticMesh(Plane.Object);
	Surface->SetRelativeLocationAndRotation(FVector(0, 0, 105), FRotator(-90.f, 0, 0));
	Surface->SetRelativeScale3D(FVector(2.0, 1.2, 1.0));
	Surface->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Surface->SetCastShadow(false);

	Frame = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame"));
	Frame->SetupAttachment(RootComponent);
	Frame->SetStaticMesh(Cube.Object);
	Frame->SetRelativeLocation(FVector(-3.0, 0, 105));
	Frame->SetRelativeScale3D(FVector(0.05, 1.3, 2.1));

	Capture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture"));
	Capture->SetupAttachment(RootComponent);
	Capture->bCaptureEveryFrame = true;
	Capture->bCaptureOnMovement = false;
	Capture->bAlwaysPersistRenderingState = true;
	// Linear scene colour: the mirror pixels are then tonemapped once, together with the rest of the frame.
	Capture->CaptureSource = ESceneCaptureSource::SCS_SceneColorHDR;
	Capture->bEnableClipPlane = true; // requires r.AllowGlobalClipPlane=1
}

void APDMirror::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	// Loaded here (not in the constructor) so the material can be generated by Scripts/build_lab.py.
	if (UMaterialInterface* Mat = LoadObject<UMaterialInterface>(nullptr, TEXT("/Game/PartialDive/Materials/M_PDMirror.M_PDMirror")))
	{
		Surface->SetMaterial(0, Mat);
	}
}

void APDMirror::BeginPlay()
{
	Super::BeginPlay();

	FVector2D ViewSize(16.0, 9.0);
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewSize);
	}
	const double Aspect = ViewSize.Y > 0.0 ? ViewSize.X / ViewSize.Y : 16.0 / 9.0;
	const int32 W = Aspect >= 1.0 ? Resolution : FMath::RoundToInt(Resolution * Aspect);
	const int32 H = Aspect >= 1.0 ? FMath::RoundToInt(Resolution / Aspect) : Resolution;
	Target = UKismetRenderingLibrary::CreateRenderTarget2D(this, W, H, RTF_RGBA16f);
	Capture->TextureTarget = Target;
	Capture->HiddenComponents.Add(Surface);
	Capture->HiddenComponents.Add(Frame);

	SurfaceMID = Surface->CreateDynamicMaterialInstance(0);
	if (SurfaceMID)
	{
		SurfaceMID->SetTextureParameterValue(TEXT("Capture"), Target);
	}
}

void APDMirror::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	const APlayerController* PC = GetWorld()->GetFirstPlayerController();
	if (!PC || !PC->PlayerCameraManager)
	{
		return;
	}
	const APlayerCameraManager* Cam = PC->PlayerCameraManager;
	const FVector Eye = Cam->GetCameraLocation();
	const FQuat EyeRot = Cam->GetCameraRotation().Quaternion();

	// Mirror plane: through the surface, normal pointing out of the glass toward the viewer side.
	const FVector P0 = Surface->GetComponentLocation();
	const FVector N = Surface->GetUpVector();
	const auto Reflect = [&N](const FVector& V) { return V - 2.0 * FVector::DotProduct(V, N) * N; };

	const FVector MirroredEye = Eye - 2.0 * FVector::DotProduct(Eye - P0, N) * N;
	const FVector Forward = Reflect(EyeRot.GetForwardVector());
	const FVector Up = Reflect(EyeRot.GetUpVector());
	Capture->SetWorldLocationAndRotation(MirroredEye, FRotationMatrix::MakeFromXZ(Forward, Up).ToQuat());
	Capture->FOVAngle = Cam->GetFOVAngle();
	Capture->ClipPlaneBase = P0;
	Capture->ClipPlaneNormal = N; // keep only what is in front of the glass
}
