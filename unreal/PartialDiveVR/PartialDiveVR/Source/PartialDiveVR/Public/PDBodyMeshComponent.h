// Visible avatar body: a poseable mesh that copies the animated driver mesh each frame, then lets
// the avatar apply procedural edits (head look, arm IK, finger curl) in component space.
#pragma once

#include "CoreMinimal.h"
#include "Components/PoseableMeshComponent.h"
#include "PDBodyMeshComponent.generated.h"

UCLASS(ClassGroup = (PartialDive), meta = (BlueprintSpawnableComponent))
class PARTIALDIVEVR_API UPDBodyMeshComponent : public UPoseableMeshComponent
{
	GENERATED_BODY()

public:
	UPDBodyMeshComponent();

	/** Animated mesh to copy from (same skeletal mesh asset). */
	UPROPERTY() TObjectPtr<USkeletalMeshComponent> Driver;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// ---- component-space helpers for the procedural layer ----
	int32 BoneIndex(FName Bone) const;
	FTransform GetCS(int32 Bone) const;
	/** Set a bone's component-space rotation (its children follow). Recomputes component space. */
	void SetCSRotation(int32 Bone, const FQuat& NewRotation);
	/** Rotate a bone in component space about its own pivot. */
	void RotateCS(int32 Bone, const FQuat& Delta) { SetCSRotation(Bone, Delta * GetCS(Bone).GetRotation()); }
	/** Reference (bind) pose component-space transform, independent of animation. */
	FTransform GetRefPoseCS(FName Bone) const;
};
