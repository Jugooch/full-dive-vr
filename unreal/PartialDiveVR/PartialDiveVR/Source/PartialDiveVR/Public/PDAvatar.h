// Full-body first-person avatar for the partial-dive experiments.
//
// Pose pipeline (no Blueprint graph editing needed):
//   GetMesh()  = hidden "driver" mannequin running ABP_Unarmed (idle/walk from CharacterMovement)
//   Body       = visible UPDBodyMeshComponent: copies the driver pose every frame, then applies
//                head look, arm IK to Touch controllers (when tracked) and finger curl from grab intent.
// Input: IntentFrames from the PartialDiveBridge (EMG/EEG/...) or controllers/keyboard (baselines).
// Reclined use: Recenter() makes the user's current physical gaze the avatar's straight-ahead.
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "PDBridgeTypes.h"
#include "PDAvatar.generated.h"

class UCameraComponent;
class UCapsuleComponent;
class UMotionControllerComponent;
class UPDBodyMeshComponent;
class UPartialDiveBridgeSubsystem;
class USpringArmComponent;

UENUM(BlueprintType)
enum class EPDArmSource : uint8
{
	/** Tracked when the Touch controller is tracked, otherwise animated. */
	Auto,
	/** Always follow the motion controllers (two-bone IK). */
	Tracked,
	/** Arms come from the animation only (EMG intent experiments: hands at rest). */
	Animated
};

UENUM(BlueprintType)
enum class EPDViewMode : uint8
{
	FirstPerson,
	/** Debug: camera in front of the avatar, looking back at it. */
	ThirdPersonFront,
	/** Debug: camera behind the avatar. */
	ThirdPersonBack,
	/** Debug: close-up of the right hand from the front (check hand closure / finger curl). */
	HandCloseUp
};

/** One body region that turns virtual contact into a HapticEvent. */
USTRUCT()
struct FPDTouchZone
{
	GENERATED_BODY()

	UPROPERTY() TObjectPtr<UCapsuleComponent> Capsule = nullptr;
	EPDHapticZone Zone = EPDHapticZone::Core;
	double LastFired = -1.0;
};

UCLASS(Config = Game)
class PARTIALDIVEVR_API APDAvatar : public ACharacter
{
	GENERATED_BODY()

public:
	APDAvatar(const FObjectInitializer& ObjectInitializer);

	// ---------------- components ----------------
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<USceneComponent> VROrigin;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<UCameraComponent> Camera;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<UMotionControllerComponent> LeftController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<UMotionControllerComponent> RightController;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<UPDBodyMeshComponent> Body;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<USpringArmComponent> DebugBoom;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Avatar") TObjectPtr<UCameraComponent> DebugCamera;

	// ---------------- tuning ----------------
	/** Eye point relative to the head bone, in actor axes (X forward, Z up), cm. */
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|View") FVector EyeOffsetFromHead = FVector(11.0, 0.0, 8.0);
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|View") float HeadYawLimitDeg = 75.f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|View") float HeadPitchLimitDeg = 60.f;
	/** Seconds after possession before the automatic first recenter (lets HMD tracking settle). */
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|View") float AutoRecenterDelay = 1.0f;

	/** Matches the decoder's walk_forward = speed / 1.6 m/s. */
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Locomotion") float MaxWalkSpeedCm = 160.f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Locomotion") float IntentTurnRateDeg = 45.f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Locomotion") float SnapTurnDeg = 30.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Config, Category = "Avatar|Arms") EPDArmSource ArmSource = EPDArmSource::Auto;
	/** Per-joint finger curl at grab = 1 (proximal, middle, distal), degrees. */
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Hands") FVector FingerCurlDeg = FVector(65.0, 85.0, 55.0);
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Hands") FVector ThumbCurlDeg = FVector(25.0, 35.0, 35.0);
	/** How fast the visible hand follows the grab value (1/s). */
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Hands") float HandResponse = 25.f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Hands") float GrabOnThreshold = 0.6f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Hands") float GrabOffThreshold = 0.35f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Hands") float GrabRadiusCm = 18.f;

	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Touch") float TouchCooldownSeconds = 0.15f;
	UPROPERTY(EditAnywhere, Config, Category = "Avatar|Touch") int32 TouchDurationMs = 120;

	// ---------------- API ----------------
	/** Make the user's current physical head pose the avatar's neutral straight-ahead (pitch+yaw; roll ignored). */
	UFUNCTION(BlueprintCallable, Category = "Avatar") void Recenter();
	UFUNCTION(BlueprintCallable, Category = "Avatar") void SetViewMode(EPDViewMode NewMode);
	UFUNCTION(BlueprintPure, Category = "Avatar") EPDViewMode GetViewMode() const { return ViewMode; }
	/** Smoothed 0..1 hand closure actually shown on the avatar. */
	UFUNCTION(BlueprintPure, Category = "Avatar") float GetHandClosure(bool bRight) const { return bRight ? HandClosureR : HandClosureL; }
	UFUNCTION(BlueprintPure, Category = "Avatar") AActor* GetHeldActor(bool bRight) const { return bRight ? HeldR : HeldL; }
	/** Dev override for testing without sensors: <0 disables. */
	void SetDebugGrab(float Left, float Right) { DebugGrabL = Left; DebugGrabR = Right; }

	/** Dev: log fingertip geometry (relaxed animated pose vs. shown pose) to verify curl direction. */
	void LogHandStats(bool bRight) const;

	/** Called by Body after it copied the driver pose; applies all procedural modifications. */
	void ApplyProceduralPose(float DeltaSeconds);

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void PossessedBy(AController* NewController) override;

private:
	UPartialDiveBridgeSubsystem* Bridge() const;
	bool IsHmdActive() const;
	void ComputeEyePoint();
	void ReadInputs(float DeltaSeconds, float& OutWalk, float& OutStrafe, float& OutTurnRate, float& OutGrabL, float& OutGrabR);
	void UpdateGrabState(bool bRight, float Value);
	void TryGrab(bool bRight);
	void Release(bool bRight);
	void BuildTouchZones();
	void ApplyHeadLook();
	void ApplyArmIK(bool bRight);
	void ApplyFingerCurl(bool bRight, float Alpha);

	UFUNCTION() void HandleBlockStart(const FPDBlockInfo& Block);
	UFUNCTION() void HandleTouchBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY() TArray<FPDTouchZone> TouchZones;
	UPROPERTY() TObjectPtr<AActor> HeldL = nullptr;
	UPROPERTY() TObjectPtr<AActor> HeldR = nullptr;

	EPDViewMode ViewMode = EPDViewMode::FirstPerson;
	FVector EyeLocal = FVector(0, 0, 70);   // actor space
	bool bRecentered = false;
	double PossessedAt = -1.0;

	float HandClosureL = 0.f, HandClosureR = 0.f;
	bool bGrabbingL = false, bGrabbingR = false;
	float DebugGrabL = -1.f, DebugGrabR = -1.f;
	float SnapTurnCooldown = 0.f;
	float DesktopPitch = 0.f;
};
