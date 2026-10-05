#include "PDBodyMeshComponent.h"

#include "Components/SkeletalMeshComponent.h"
#include "Engine/SkinnedAsset.h"
#include "PDAvatar.h"
#include "ReferenceSkeleton.h"

UPDBodyMeshComponent::UPDBodyMeshComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	// After the driver's (possibly parallel) animation evaluation has completed this frame.
	PrimaryComponentTick.TickGroup = TG_PostPhysics;
}

void UPDBodyMeshComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!Driver || !GetSkinnedAsset() || BoneSpaceTransforms.Num() == 0)
	{
		return;
	}
	// Start from the animated pose EVERY frame; procedural edits must never accumulate.
	const TArray<FTransform>& Animated = Driver->GetBoneSpaceTransforms();
	if (Animated.Num() == BoneSpaceTransforms.Num())
	{
		BoneSpaceTransforms = Animated;
	}
	else
	{
		CopyPoseFromSkeletalComponent(Driver);
	}
	FillComponentSpaceTransforms();
	if (APDAvatar* Avatar = Cast<APDAvatar>(GetOwner()))
	{
		Avatar->ApplyProceduralPose(DeltaTime);
	}
	MarkRefreshTransformDirty();
	RefreshBoneTransforms();
}

int32 UPDBodyMeshComponent::BoneIndex(FName Bone) const
{
	return GetSkinnedAsset() ? GetSkinnedAsset()->GetRefSkeleton().FindBoneIndex(Bone) : INDEX_NONE;
}

FTransform UPDBodyMeshComponent::GetCS(int32 Bone) const
{
	const TArray<FTransform>& CS = GetComponentSpaceTransforms();
	return CS.IsValidIndex(Bone) ? CS[Bone] : FTransform::Identity;
}

void UPDBodyMeshComponent::SetCSRotation(int32 Bone, const FQuat& NewRotation)
{
	if (!BoneSpaceTransforms.IsValidIndex(Bone))
	{
		return;
	}
	const int32 Parent = GetSkinnedAsset()->GetRefSkeleton().GetParentIndex(Bone);
	const FQuat ParentRot = Parent == INDEX_NONE ? FQuat::Identity : GetCS(Parent).GetRotation();
	BoneSpaceTransforms[Bone].SetRotation((ParentRot.Inverse() * NewRotation).GetNormalized());
	FillComponentSpaceTransforms();
}

FTransform UPDBodyMeshComponent::GetRefPoseCS(FName Bone) const
{
	if (!GetSkinnedAsset())
	{
		return FTransform::Identity;
	}
	const FReferenceSkeleton& Ref = GetSkinnedAsset()->GetRefSkeleton();
	int32 Index = Ref.FindBoneIndex(Bone);
	FTransform Result = FTransform::Identity;
	while (Index != INDEX_NONE)
	{
		Result = Result * Ref.GetRefBonePose()[Index];
		Index = Ref.GetParentIndex(Index);
	}
	return Result;
}
