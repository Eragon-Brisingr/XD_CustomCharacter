// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimNode_CustomSkeleton.h"
#include <AnimationRuntime.h>
#include <Animation/AnimInstanceProxy.h>

void FAnimNode_CustomSkeleton::Initialize_AnyThread(const FAnimationInitializeContext& Context)
{
	BasePose.Initialize(Context);
}

void FAnimNode_CustomSkeleton::Update_AnyThread(const FAnimationUpdateContext& Context)
{
	GetEvaluateGraphExposedInputs().Execute(Context);
	BasePose.Update(Context);

	const FBoneContainer& RequiredBones = Context.AnimInstanceProxy->GetRequiredBones();

	if (!CustomCharacterRuntimeDataRef)
	{
		return;
	}

	if (CachedConfig.Get() == CustomCharacterRuntimeDataRef->CustomConfig)
	{
		return;
	}

	CustomBoneDatas.Empty();
	CachedConfig = CustomCharacterRuntimeDataRef->CustomConfig;

	for (int32 Idx = 0; Idx < CustomCharacterRuntimeDataRef->CustomConfig->SkeletonData.Num(); ++Idx)
	{
		const FCustomSkeletonEntry& CustomSkeletonEntry = CustomCharacterRuntimeDataRef->CustomConfig->SkeletonData[Idx];
		if (CustomSkeletonEntry.bNotEffectChildBone)
		{
			for (const FCustomSkeletonBoneData& BoneData : CustomSkeletonEntry.BoneDatas)
			{
				const FName& BoneName = BoneData.BoneName;

				int32 Index = CustomBoneNoChildDatas.IndexOfByPredicate([&](const FCustomSkeletonRuntimeNoChildEntry& E) {return E.BoneReference.BoneName == BoneName; });
				if (Index == INDEX_NONE)
				{
					FBoneReference BoneReference(BoneName);
					if (BoneReference.Initialize(RequiredBones))
					{
						FCustomSkeletonRuntimeNoChildEntry CustomSkeletonRuntimeEntry;
						CustomSkeletonRuntimeEntry.BoneReference = BoneReference;

						struct FBoneHelper
						{
							static void GetDirectChildBones(const FReferenceSkeleton& ReferenceSkeleton, int32 ParentBoneIndex, TArray<int32>& Children)
							{
								Children.Reset();

								const int32 NumBones = ReferenceSkeleton.GetNum();
								for (int32 ChildIndex = ParentBoneIndex + 1; ChildIndex < NumBones; ChildIndex++)
								{
									if (ParentBoneIndex == ReferenceSkeleton.GetParentIndex(ChildIndex))
									{
										Children.Add(ChildIndex);
									}
								}
							}
						};

						FBoneHelper::GetDirectChildBones(RequiredBones.GetReferenceSkeleton(), BoneReference.BoneIndex, CustomSkeletonRuntimeEntry.ChildBones);

						Index = CustomBoneNoChildDatas.Add(CustomSkeletonRuntimeEntry);
					}
					else
					{
						continue;
					}
				}
				FCustomSkeletonRuntimeNoChildEntry& Entry = CustomBoneNoChildDatas[Index];

				switch (BoneData.Mode)
				{
				case ECustomSkeletonMode::Offset:
					Entry.OffsetModifies.Add({ BoneData.ApplyAxis.GetSafeNormal(), Idx });
					break;
				case ECustomSkeletonMode::Scale:
					Entry.ScaleModifies.Add({ BoneData.ApplyAxis.GetSafeNormal(), Idx });
					break;
				case ECustomSkeletonMode::Rotation:
					Entry.RotationModifies.Add({ BoneData.ApplyAxis.GetSafeNormal(), Idx });
					break;
				}
			}
		}
		else
		{
			for (const FCustomSkeletonBoneData& BoneData : CustomSkeletonEntry.BoneDatas)
			{
				const FName& BoneName = BoneData.BoneName;

				int32 Index = CustomBoneDatas.IndexOfByPredicate([&](const FCustomSkeletonRuntimeEntry& E) {return E.BoneReference.BoneName == BoneName; });
				if (Index == INDEX_NONE)
				{
					FBoneReference BoneReferences(BoneName);
					if (BoneReferences.Initialize(RequiredBones))
					{
						FCustomSkeletonRuntimeEntry CustomSkeletonRuntimeEntry;
						CustomSkeletonRuntimeEntry.BoneReference = BoneReferences;
						Index = CustomBoneDatas.Add(CustomSkeletonRuntimeEntry);
					}
					else
					{
						continue;
					}
				}
				FCustomSkeletonRuntimeEntry& Entry = CustomBoneDatas[Index];

				switch (BoneData.Mode)
				{
				case ECustomSkeletonMode::Offset:
					Entry.OffsetModifies.Add({ BoneData.ApplyAxis.GetSafeNormal(), Idx });
					break;
				case ECustomSkeletonMode::Scale:
					Entry.ScaleModifies.Add({ BoneData.ApplyAxis.GetSafeNormal(), Idx });
					break;
				case ECustomSkeletonMode::Rotation:
					Entry.RotationModifies.Add({ BoneData.ApplyAxis.GetSafeNormal(), Idx });
					break;
				}
			}
		}
	}

	CustomBoneDatas.Sort([&](const FCustomSkeletonRuntimeEntry& LHS, const FCustomSkeletonRuntimeEntry& RHS)
		{
			return LHS.BoneReference.BoneIndex < RHS.BoneReference.BoneIndex;
		});
}

void FAnimNode_CustomSkeleton::Evaluate_AnyThread(FPoseContext& Output)
{
	BasePose.Evaluate(Output);

	if (CustomCharacterRuntimeDataRef && CustomCharacterRuntimeDataRef->CustomSkeletonValues.Num() != CustomCharacterRuntimeDataRef->CustomConfig->SkeletonData.Num())
	{
		return;
	}

	// the way we apply transform is same as FMatrix or FTransform
	// we apply scale first, and rotation, and translation
	// if you'd like to translate first, you'll need two nodes that first node does translate and second nodes to rotate.
	const FBoneContainer& BoneContainer = Output.Pose.GetBoneContainer();

	for (const FCustomSkeletonRuntimeEntry& Entry : CustomBoneDatas)
	{
		if (Entry.BoneReference.IsValidToEvaluate(BoneContainer))
		{
			FCompactPoseBoneIndex CompactPoseBoneToModify = Entry.BoneReference.GetCompactPoseIndex(BoneContainer);
			FTransform& NewBoneTM = Output.Pose[CompactPoseBoneToModify];

			if (Entry.ScaleModifies.Num() > 0)
			{
				FVector Scale{ FVector::ZeroVector };
				for (const auto& Data : Entry.ScaleModifies)
				{
					Scale += Data.Key * CustomCharacterRuntimeDataRef->GetCustomSkeletonValueScaled(Data.Value);
				}
				NewBoneTM.SetScale3D(NewBoneTM.GetScale3D() + Scale);
			}

			if (Entry.RotationModifies.Num() > 0)
			{
				FVector Rotation{ FVector::ZeroVector };
				for (const auto& Data : Entry.RotationModifies)
				{
					Rotation += Data.Key * CustomCharacterRuntimeDataRef->GetCustomSkeletonValueScaled(Data.Value);
				}
				NewBoneTM.SetRotation(FQuat(FRotator(Rotation.X, Rotation.Y, Rotation.Z)) * NewBoneTM.GetRotation());
			}

			if (Entry.OffsetModifies.Num() > 0)
			{
				FVector Offset{ FVector::ZeroVector };
				for (const auto& Data : Entry.OffsetModifies)
				{
					Offset += Data.Key * CustomCharacterRuntimeDataRef->GetCustomSkeletonValueScaled(Data.Value);
				}
				NewBoneTM.AddToTranslation(Offset);
			}
		}
	}

	for (const FCustomSkeletonRuntimeNoChildEntry& Entry : CustomBoneNoChildDatas)
	{
		if (Entry.BoneReference.IsValidToEvaluate(BoneContainer))
		{
			FCompactPoseBoneIndex CompactPoseBoneToModify = Entry.BoneReference.GetCompactPoseIndex(BoneContainer);
			FTransform& NewBoneTM = Output.Pose[CompactPoseBoneToModify];

			if (Entry.ScaleModifies.Num() > 0)
			{
				FVector Scale{ FVector::ZeroVector };
				for (const auto& Data : Entry.ScaleModifies)
				{
					Scale += Data.Key * CustomCharacterRuntimeDataRef->GetCustomSkeletonValueScaled(Data.Value);
				}
				for (int32 ChildBoneIdx : Entry.ChildBones)
				{
					FTransform& ChildBoneTM = Output.Pose[FCompactPoseBoneIndex(ChildBoneIdx)];
					FVector ClildScale3D = ChildBoneTM.GetScale3D();
					ChildBoneTM.SetScale3D(ClildScale3D / (ClildScale3D + Scale));
				}
				NewBoneTM.SetScale3D(NewBoneTM.GetScale3D() + Scale);
			}

			if (Entry.RotationModifies.Num() > 0)
			{
				FVector VectorRotation{ FVector::ZeroVector };
				for (const auto& Data : Entry.RotationModifies)
				{
					VectorRotation += Data.Key * CustomCharacterRuntimeDataRef->GetCustomSkeletonValueScaled(Data.Value);
				}
				FQuat Rotation = FQuat(FRotator(VectorRotation.X, VectorRotation.Y, VectorRotation.Z));
				for (int32 ChildBoneIdx : Entry.ChildBones)
				{
					FTransform& ChildBoneTM = Output.Pose[FCompactPoseBoneIndex(ChildBoneIdx)];
					ChildBoneTM.SetRotation(Rotation.Inverse() * ChildBoneTM.GetRotation());
				}
				NewBoneTM.SetRotation(Rotation * NewBoneTM.GetRotation());
			}

			if (Entry.OffsetModifies.Num() > 0)
			{
				FVector Offset{ FVector::ZeroVector };
				for (const auto& Data : Entry.OffsetModifies)
				{
					Offset += Data.Key * CustomCharacterRuntimeDataRef->GetCustomSkeletonValueScaled(Data.Value);
				}
				for (int32 ChildBoneIdx : Entry.ChildBones)
				{
					FTransform& ChildBoneTM = Output.Pose[FCompactPoseBoneIndex(ChildBoneIdx)];
					NewBoneTM.AddToTranslation(-Offset);
				}
				NewBoneTM.AddToTranslation(Offset);
			}
		}
	}
}

void FAnimNode_CustomSkeleton::CacheBones_AnyThread(const FAnimationCacheBonesContext& Context)
{
	Super::CacheBones_AnyThread(Context);
}
