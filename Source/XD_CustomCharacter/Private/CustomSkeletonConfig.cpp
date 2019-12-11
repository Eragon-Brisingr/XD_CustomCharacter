// Fill out your copyright notice in the Description page of Project Settings.


#include "CustomSkeletonConfig.h"
#include <Components/SkeletalMeshComponent.h>
#include "Materials/MaterialInstanceDynamic.h"

FText FCustomSkeletonEntry::GetBonesDesc() const
{
	FString Desc = TEXT("|");
	for (const FCustomSkeletonBoneData& BoneData : BoneDatas)
	{
		Desc += BoneData.BoneName.ToString() + TEXT("|");
	}
	return FText::FromString(Desc);
}

FText FCustomMorphEntry::GetMorphsDesc() const
{
	FString Desc = TEXT("|");
	for (const FName& MorphTargetName : MorphTargetNames)
	{
		Desc += MorphTargetName.ToString() + TEXT("|");
	}
	return FText::FromString(Desc);
}

namespace
{
	bool operator==(const FText& LHS, const FText& RHS) { return LHS.EqualTo(RHS); }
}

TArray<FText> UCustomCharacterConfig::GetAllCategoryNames() const
{
	TArray<FText> TempRes;
	for (const FCustomSkeletonEntry& Entry : SkeletonData)
	{
		TempRes.AddUnique(Entry.Category);
	}
	for (const FCustomMorphEntry& Entry : MorphData)
	{
		TempRes.AddUnique(Entry.Category);
	}
	for (const FCustomMaterialFloatEntry& Entry : MaterialFloatData)
	{
		TempRes.AddUnique(Entry.Category);
	}
	for (const FCustomMaterialColorEntry& Entry : MaterialColorData)
	{
		TempRes.AddUnique(Entry.Category);
	}
	for (const FCustomMaterialTextureEntry& Entry : MaterialTextureData)
	{
		TempRes.AddUnique(Entry.Category);
	}

	return TempRes;
}

void FCustomCharacterRuntimeData::SyncConfigSize()
{
	if (CustomConfig)
	{
		CustomSkeletonValues.SetNumZeroed(CustomConfig->SkeletonData.Num());
		CustomMorphValues.SetNumZeroed(CustomConfig->MorphData.Num());
		CustomMaterialFloatValues.SetNumZeroed(CustomConfig->MaterialFloatData.Num());
		CustomMaterialColorValues.SetNumZeroed(CustomConfig->MaterialColorData.Num());
		CustomMaterialTextureValues.SetNumZeroed(CustomConfig->MaterialTextureData.Num());
	}
}

float FCustomCharacterRuntimeData::GetCustomSkeletonValue(int32 Idx) const
{
	const FCustomSkeletonEntry& CustomSkeletonEntry = CustomConfig->SkeletonData[Idx];
	return CustomSkeletonValues[Idx].GetValue(CustomSkeletonEntry.MinValue, CustomSkeletonEntry.MaxValue);
}

float FCustomCharacterRuntimeData::GetCustomSkeletonValueScaled(int32 Idx) const
{
	const FCustomSkeletonEntry& CustomSkeletonEntry = CustomConfig->SkeletonData[Idx];
	return GetCustomSkeletonValue(Idx) * CustomSkeletonEntry.Scale;
}

void FCustomCharacterRuntimeData::SetCustomSkeletonValue(int32 Idx, float InValue)
{
	const FCustomSkeletonEntry& CustomSkeletonEntry = CustomConfig->SkeletonData[Idx];
	CustomSkeletonValues[Idx].SetValue(InValue, CustomSkeletonEntry.MinValue, CustomSkeletonEntry.MaxValue);
}

float FCustomCharacterRuntimeData::GetCustomMorphValue(int32 Idx) const
{
	const FCustomMorphEntry& CustomMorphEntry = CustomConfig->MorphData[Idx];
	return CustomMorphValues[Idx].GetValue(CustomMorphEntry.MinValue, CustomMorphEntry.MaxValue);
}

void FCustomCharacterRuntimeData::SetCustomMorphValue(int32 Idx, float InValue)
{
	const FCustomMorphEntry& CustomMorphEntry = CustomConfig->MorphData[Idx];
	CustomMorphValues[Idx].SetValue(InValue, CustomMorphEntry.MinValue, CustomMorphEntry.MaxValue);
}

void FCustomCharacterRuntimeData::ApplyMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	for (int32 Idx = 0; Idx < CustomMorphValues.Num(); ++Idx)
	{
		const FCustomMorphEntry& Entry = CustomConfig->MorphData[Idx];
		float Value = GetCustomMorphValue(Idx);
		for (const FName& MorphTargetName : Entry.MorphTargetNames)
		{
			SkeletalMeshComponent->SetMorphTarget(MorphTargetName, Value);
		}
	}
}

void FCustomCharacterRuntimeData::ApplyMaterialFloatValues(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	if (!CustomConfig)
	{
		return;
	}

	struct FLinearColorTypeData
	{
		int32 IdxR = INDEX_NONE;
		int32 IdxG = INDEX_NONE;
		int32 IdxB = INDEX_NONE;
		int32 IdxA = INDEX_NONE;
	};

	TMap<int32, TMap<FName, FLinearColorTypeData>> ColorData;

	for (int32 Idx = 0; Idx < CustomMaterialFloatValues.Num(); ++Idx)
	{
		const float Value = CustomMaterialFloatValues[Idx];
		const FCustomMaterialFloatEntry& Entry = CustomConfig->MaterialFloatData[Idx];
		for (const FCustomMaterialFloatData& CustomMaterialFloatData : Entry.CustomMaterialFloatDatas)
		{
			switch (CustomMaterialFloatData.CustomMaterialFloatType)
			{
			case ECustomMaterialFloatType::Float:
				GetMID(SkeletalMeshComponent, CustomMaterialFloatData.SlotId)->SetScalarParameterValue(CustomMaterialFloatData.ParameterName, Value);
				break;
			case ECustomMaterialFloatType::ChannelR:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotId).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxR = Idx;
				break;
			case ECustomMaterialFloatType::ChannelG:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotId).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxG = Idx;
				break;
			case ECustomMaterialFloatType::ChannelB:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotId).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxB = Idx;
				break;
			case ECustomMaterialFloatType::ChannelA:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotId).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxA = Idx;
				break;
			}
		}
	}

	for (const auto& IdAndData : ColorData)
	{
		if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, IdAndData.Key))
		{
			for (const auto& ParameterNameAndData : IdAndData.Value)
			{
				FLinearColor Color = MaterialInstanceDynamic->K2_GetVectorParameterValue(ParameterNameAndData.Key);
				const FLinearColorTypeData& Data = ParameterNameAndData.Value;
				if (Data.IdxR != INDEX_NONE)
				{
					Color.R = CustomMaterialFloatValues[Data.IdxR];
				}
				if (Data.IdxG != INDEX_NONE)
				{
					Color.G = CustomMaterialFloatValues[Data.IdxG];
				}
				if (Data.IdxB != INDEX_NONE)
				{
					Color.B = CustomMaterialFloatValues[Data.IdxB];
				}
				if (Data.IdxA != INDEX_NONE)
				{
					Color.A = CustomMaterialFloatValues[Data.IdxA];
				}
				MaterialInstanceDynamic->SetVectorParameterValue(ParameterNameAndData.Key, Color);
			}
		}
	}
}

void FCustomCharacterRuntimeData::ApplyMaterialColorValues(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	if (!CustomConfig)
	{
		return;
	}

	for (int32 Idx = 0; Idx < CustomMaterialColorValues.Num(); ++Idx)
	{
		const FCustomMaterialColorEntry& Entry = CustomConfig->MaterialColorData[Idx];
		for (const FCustomMaterialColorData& CustomMaterialColorData : Entry.CustomMaterialColorDatas)
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, CustomMaterialColorData.SlotId))
			{
				MaterialInstanceDynamic->SetVectorParameterValue(CustomMaterialColorData.ParameterName, CustomMaterialColorValues[Idx]);
			}
		}
	}
}

void FCustomCharacterRuntimeData::ApplyMaterialTextureValues(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	if (!CustomConfig)
	{
		return;
	}

	for (int32 Idx = 0; Idx < CustomMaterialTextureValues.Num(); ++Idx)
	{
		const FCustomMaterialTextureEntry& Entry = CustomConfig->MaterialTextureData[Idx];
		for (const FCustomMaterialTextureData& CustomMaterialColorData : Entry.CustomMaterialTextureDatas)
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, CustomMaterialColorData.SlotId))
			{
				MaterialInstanceDynamic->SetTextureParameterValue(CustomMaterialColorData.ParameterName, CustomMaterialTextureValues[Idx]);
			}
		}
	}
}

void FCustomCharacterRuntimeData::ApplyAllMaterialData(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	ApplyMaterialFloatValues(SkeletalMeshComponent);
	ApplyMaterialColorValues(SkeletalMeshComponent);
	ApplyMaterialTextureValues(SkeletalMeshComponent);
}

UMaterialInstanceDynamic* FCustomCharacterRuntimeData::GetMID(USkeletalMeshComponent* SkeletalMeshComponent, int32 Idx) const
{
	UMaterialInstanceDynamic*& MID = MIDMap.FindOrAdd(Idx);
	if (!MID)
	{
		MID = SkeletalMeshComponent->CreateDynamicMaterialInstance(Idx);
	}
	return MID;
}
