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

void FCustomCharacterRuntimeData::SetCustomCharacterRuntimeData(const FCustomCharacterRuntimeData& NewCustomCharacterRuntimeData)
{
	if (CustomConfig != NewCustomCharacterRuntimeData.CustomConfig)
	{
		*this = NewCustomCharacterRuntimeData;
	}
}

void FCustomCharacterRuntimeData::SyncConfigData(UCustomCharacterConfig* OldCustomConfig)
{
	if (CustomConfig)
	{
		if (CustomConfig == OldCustomConfig)
		{
			CustomSkeletonValues.SetNum(CustomConfig->SkeletonData.Num());
			CustomMorphValues.SetNum(CustomConfig->MorphData.Num());
			CustomMaterialFloatValues.SetNum(CustomConfig->MaterialFloatData.Num());
			CustomMaterialColorValues.SetNum(CustomConfig->MaterialColorData.Num());
			CustomMaterialTextureValues.SetNum(CustomConfig->MaterialTextureData.Num());
		}
		else
		{
			{
				const auto& CurDataTemplate = CustomConfig->SkeletonData;
				auto& Datas = CustomSkeletonValues;
				auto OldDatas = Datas;
				Datas.SetNumUninitialized(CurDataTemplate.Num());
				for (int32 Idx = 0; Idx < Datas.Num(); ++Idx)
				{
					const int32 OldValueIdx = OldCustomConfig ? OldCustomConfig->SkeletonData.IndexOfByPredicate([&](const auto& E) {return E.DisplayName.EqualTo(CurDataTemplate[Idx].DisplayName); }) : INDEX_NONE;
					if (OldValueIdx != INDEX_NONE)
					{
						Datas[Idx] = OldDatas[Idx];
					}
					else
					{
						Datas[Idx].SetValue(CurDataTemplate[Idx].DefaultValue, CurDataTemplate[Idx].MaxValue, CurDataTemplate[Idx].MinValue);
					}
				}
			}

			{
				const auto& CurDataTemplate = CustomConfig->MorphData;
				auto& Datas = CustomMorphValues;
				auto OldDatas = Datas;
				Datas.SetNumUninitialized(CurDataTemplate.Num());
				for (int32 Idx = 0; Idx < Datas.Num(); ++Idx)
				{
					const int32 OldValueIdx = OldCustomConfig ? OldCustomConfig->MorphData.IndexOfByPredicate([&](const auto& E) {return E.DisplayName.EqualTo(CurDataTemplate[Idx].DisplayName); }) : INDEX_NONE;
					if (OldValueIdx != INDEX_NONE)
					{
						Datas[Idx] = OldDatas[Idx];
					}
					else
					{
						Datas[Idx].SetValue(CurDataTemplate[Idx].DefaultValue, CurDataTemplate[Idx].MaxValue, CurDataTemplate[Idx].MinValue);
					}
				}
			}

			{
				const auto& CurDataTemplate = CustomConfig->MaterialFloatData;
				auto& Datas = CustomMaterialFloatValues;
				auto OldDatas = Datas;
				Datas.SetNumUninitialized(CurDataTemplate.Num());
				for (int32 Idx = 0; Idx < Datas.Num(); ++Idx)
				{
					const int32 OldValueIdx = OldCustomConfig ? OldCustomConfig->MaterialFloatData.IndexOfByPredicate([&](const auto& E) {return E.DisplayName.EqualTo(CurDataTemplate[Idx].DisplayName); }) : INDEX_NONE;
					if (OldValueIdx != INDEX_NONE)
					{
						Datas[Idx] = OldDatas[Idx];
					}
					else
					{
						Datas[Idx] = CurDataTemplate[Idx].DefaultValue;
					}
				}
			}

			{
				const auto& CurDataTemplate = CustomConfig->MaterialColorData;
				auto& Datas = CustomMaterialColorValues;
				auto OldDatas = Datas;
				Datas.SetNumUninitialized(CurDataTemplate.Num());
				for (int32 Idx = 0; Idx < Datas.Num(); ++Idx)
				{
					const int32 OldValueIdx = OldCustomConfig ? OldCustomConfig->MaterialColorData.IndexOfByPredicate([&](const auto& E) {return E.DisplayName.EqualTo(CurDataTemplate[Idx].DisplayName); }) : INDEX_NONE;
					if (OldValueIdx != INDEX_NONE)
					{
						Datas[Idx] = OldDatas[Idx];
					}
					else
					{
						Datas[Idx] = CurDataTemplate[Idx].DefaultColor;
					}
				}
			}

			{
				const auto& CurDataTemplate = CustomConfig->MaterialTextureData;
				auto& Datas = CustomMaterialTextureValues;
				auto OldDatas = Datas;
				Datas.SetNumUninitialized(CurDataTemplate.Num());
				for (int32 Idx = 0; Idx < Datas.Num(); ++Idx)
				{
					const int32 OldValueIdx = OldCustomConfig ? OldCustomConfig->MaterialTextureData.IndexOfByPredicate([&](const auto& E) {return E.DisplayName.EqualTo(CurDataTemplate[Idx].DisplayName); }) : INDEX_NONE;
					if (OldValueIdx != INDEX_NONE)
					{
						Datas[Idx] = OldDatas[Idx];
					}
					else
					{
						Datas[Idx] = CurDataTemplate[Idx].DefaultTexture;
					}
				}
			}
		}
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
	return GetCustomSkeletonValue(Idx);
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

void FCustomCharacterRuntimeData::SetCustomMorphValue(int32 Idx, float InValue, USkeletalMeshComponent* SkeletalMeshComponent)
{
	const FCustomMorphEntry& CustomMorphEntry = CustomConfig->MorphData[Idx];
	CustomMorphValues[Idx].SetValue(InValue, CustomMorphEntry.MinValue, CustomMorphEntry.MaxValue);

	if (SkeletalMeshComponent)
	{
		for (const FName& MorphTargetName : CustomMorphEntry.MorphTargetNames)
		{
			SkeletalMeshComponent->SetMorphTarget(MorphTargetName, GetCustomMorphValue(Idx));
		}
	}
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

void FCustomCharacterRuntimeData::SetCustomMaterialFloatValue(int32 Idx, float InValue, USkeletalMeshComponent* SkeletalMeshComponent)
{
	CustomMaterialFloatValues[Idx] = InValue;

	if (SkeletalMeshComponent)
	{
		TMap<FName, TMap<FName, FLinearColorTypeData>> ColorData;

		const float Value = CustomMaterialFloatValues[Idx];
		const FCustomMaterialFloatEntry& Entry = CustomConfig->MaterialFloatData[Idx];
		for (const FCustomMaterialFloatData& CustomMaterialFloatData : Entry.CustomMaterialFloatDatas)
		{
			switch (CustomMaterialFloatData.CustomMaterialFloatType)
			{
			case ECustomMaterialFloatType::Float:
				GetMID(SkeletalMeshComponent, CustomMaterialFloatData.SlotName)->SetScalarParameterValue(CustomMaterialFloatData.ParameterName, Value);
				break;
			case ECustomMaterialFloatType::ChannelR:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxR = Idx;
				break;
			case ECustomMaterialFloatType::ChannelG:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxG = Idx;
				break;
			case ECustomMaterialFloatType::ChannelB:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxB = Idx;
				break;
			case ECustomMaterialFloatType::ChannelA:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxA = Idx;
				break;
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
}

void FCustomCharacterRuntimeData::ApplyMaterialFloatValues(USkeletalMeshComponent* SkeletalMeshComponent) const
{
	if (!CustomConfig)
	{
		return;
	}

	TMap<FName, TMap<FName, FLinearColorTypeData>> ColorData;

	for (int32 Idx = 0; Idx < CustomMaterialFloatValues.Num(); ++Idx)
	{
		const float Value = CustomMaterialFloatValues[Idx];
		const FCustomMaterialFloatEntry& Entry = CustomConfig->MaterialFloatData[Idx];
		for (const FCustomMaterialFloatData& CustomMaterialFloatData : Entry.CustomMaterialFloatDatas)
		{
			switch (CustomMaterialFloatData.CustomMaterialFloatType)
			{
			case ECustomMaterialFloatType::Float:
				GetMID(SkeletalMeshComponent, CustomMaterialFloatData.SlotName)->SetScalarParameterValue(CustomMaterialFloatData.ParameterName, Value);
				break;
			case ECustomMaterialFloatType::ChannelR:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxR = Idx;
				break;
			case ECustomMaterialFloatType::ChannelG:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxG = Idx;
				break;
			case ECustomMaterialFloatType::ChannelB:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxB = Idx;
				break;
			case ECustomMaterialFloatType::ChannelA:
				ColorData.FindOrAdd(CustomMaterialFloatData.SlotName).FindOrAdd(CustomMaterialFloatData.ParameterName).IdxA = Idx;
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

void FCustomCharacterRuntimeData::SetCustomMaterialColorValue(int32 Idx, const FLinearColor& InValue, USkeletalMeshComponent* SkeletalMeshComponent)
{
	CustomMaterialColorValues[Idx] = InValue;

	if (SkeletalMeshComponent)
	{
		const FCustomMaterialColorEntry& Entry = CustomConfig->MaterialColorData[Idx];
		for (const FCustomMaterialColorData& CustomMaterialColorData : Entry.CustomMaterialColorDatas)
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, CustomMaterialColorData.SlotName))
			{
				MaterialInstanceDynamic->SetVectorParameterValue(CustomMaterialColorData.ParameterName, CustomMaterialColorValues[Idx]);
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
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, CustomMaterialColorData.SlotName))
			{
				MaterialInstanceDynamic->SetVectorParameterValue(CustomMaterialColorData.ParameterName, CustomMaterialColorValues[Idx]);
			}
		}
	}
}

void FCustomCharacterRuntimeData::SetCustomMaterialTextureValue(int32 Idx, UTexture* InValue, USkeletalMeshComponent* SkeletalMeshComponent)
{
	CustomMaterialTextureValues[Idx] = InValue;

	if (SkeletalMeshComponent)
	{
		const FCustomMaterialTextureEntry& Entry = CustomConfig->MaterialTextureData[Idx];
		for (const FCustomMaterialTextureData& CustomMaterialColorData : Entry.CustomMaterialTextureDatas)
		{
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, CustomMaterialColorData.SlotName))
			{
				MaterialInstanceDynamic->SetTextureParameterValue(CustomMaterialColorData.ParameterName, CustomMaterialTextureValues[Idx]);
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
			if (UMaterialInstanceDynamic* MaterialInstanceDynamic = GetMID(SkeletalMeshComponent, CustomMaterialColorData.SlotName))
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

UMaterialInstanceDynamic* FCustomCharacterRuntimeData::GetMID(USkeletalMeshComponent* SkeletalMeshComponent, const FName& SlotName) const
{
	const int32 Idx = SkeletalMeshComponent->GetMaterialIndex(SlotName);
	return SkeletalMeshComponent->CreateDynamicMaterialInstance(Idx, nullptr, *(TEXT("DMI_") + SlotName.ToString()));
}
