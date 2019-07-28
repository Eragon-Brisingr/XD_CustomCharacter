// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "CustomSkeletonConfig.generated.h"

class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class UTexture;

/**
 *
 */
USTRUCT()
struct XD_CUSTOMCHARACTER_API FCustomCharacterRuntimeEntry
{
	GENERATED_BODY()
public:
	FCustomCharacterRuntimeEntry() = default;

private:
	UPROPERTY(EditAnywhere, Category = "角色定制", SaveGame)
	float Value;
public:
	void SetValue(float InValue, float MinValue, float MaxValue)
	{
		Value = InValue;
	}
	float GetValue(float MinValue, float MaxValue) const
	{
		return Value;
	}
};

UENUM()
enum class ECustomSkeletonMode
{
	Offset,
	Rotation,
	Scale
};

USTRUCT(BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomSkeletonBoneData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "骨架定制")
	FName BoneName;

	UPROPERTY(EditAnywhere, Category = "骨架定制")
	ECustomSkeletonMode Mode;

	UPROPERTY(EditAnywhere, Category = "骨架定制")
	FVector ApplyAxis = FVector(0.f, 1.f, 0.f);
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomSkeletonEntry
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "骨架定制")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "骨架定制")
	FText Category;
	UPROPERTY(EditAnywhere, Category = "骨架定制")
	TArray<FCustomSkeletonBoneData> BoneDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "骨架定制")
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "骨架定制")
	float MinValue = -1.f;
	UPROPERTY(EditAnywhere, Category = "骨架定制")
	float Scale = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "骨架定制")
	uint8 bNotEffectChildBone : 1;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "骨架定制")
	float DefalutValue;

	FText GetBonesDesc() const;

	FCustomCharacterRuntimeEntry ToRuntimeData() const
	{
		FCustomCharacterRuntimeEntry Entry;
		Entry.SetValue(DefalutValue, MinValue, MaxValue);
		return Entry;
	}
};

USTRUCT(BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomMorphEntry
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "混合变形定制")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "混合变形定制")
	FText Category;
	UPROPERTY(EditAnywhere, Category = "混合变形定制")
	TArray<FName> MorphTargetNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "混合变形定制")
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "混合变形定制")
	float MinValue = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "混合变形定制")
	float DefalutValue;

	FText GetMorphsDesc() const;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "材质定制")
	int32 SlotId;
	UPROPERTY(EditAnywhere, Category = "材质定制")
	FName ParameterName;
};

UENUM()
enum class ECustomMaterialFloatType : uint8
{
	Float,
	ChannelR,
	ChannelG,
	ChannelB,
	ChannelA
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialFloatData : public FCustomMaterialDataBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = "材质定制")
	ECustomMaterialFloatType CustomMaterialFloatType;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialColorData : public FCustomMaterialDataBase
{
	GENERATED_BODY()
public:

};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialTextureData : public FCustomMaterialDataBase
{
	GENERATED_BODY()
public:

};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialEntryBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	FText Category;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialFloatEntry : public FCustomMaterialEntryBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	TArray<FCustomMaterialFloatData> CustomMaterialFloatDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	float MinValue = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	float DefalutValue;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialColorEntry : public FCustomMaterialEntryBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	TArray<FCustomMaterialColorData> CustomMaterialColorDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	FLinearColor DefalutColor;
};

UCLASS()
class XD_CUSTOMCHARACTER_API UCustomMaterialTextureConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	TArray<UTexture*> ReplaceTextures;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialTextureEntry : public FCustomMaterialEntryBase
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	TArray<FCustomMaterialTextureData> CustomMaterialTextureDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	UTexture* DefalutTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制")
	UCustomMaterialTextureConfig* TextureConfig;
};

UCLASS()
class XD_CUSTOMCHARACTER_API UCustomCharacterConfig : public UDataAsset
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制")
	TArray<FCustomSkeletonEntry> SkeletonData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制")
	TArray<FCustomMorphEntry> MorphData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制")
	TArray<FCustomMaterialFloatEntry> MaterialFloatData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制")
	TArray<FCustomMaterialColorEntry> MaterialColorData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制")
	TArray<FCustomMaterialTextureEntry> MaterialTextureData;

	TArray<FText> GetAllCategoryNames() const;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomCharacterRuntimeData
{
	GENERATED_BODY()
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色定制", EditFixedSize)
	UCustomCharacterConfig* CustomConfig;

	void SyncConfigSize();

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<FCustomCharacterRuntimeEntry> CustomSkeletonValues;

	float GetCustomSkeletonValue(int32 Idx) const;
	float GetCustomSkeletonValueScaled(int32 Idx) const;
	void SetCustomSkeletonValue(int32 Idx, float InValue);

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<FCustomCharacterRuntimeEntry> CustomMorphValues;

	float GetCustomMorphValue(int32 Idx) const;
	void SetCustomMorphValue(int32 Idx, float InValue);
	void ApplyMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent) const;

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<float> CustomMaterialFloatValues;

	void ApplyMaterialFloatValues(USkeletalMeshComponent* SkeletalMeshComponent) const;

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<FLinearColor> CustomMaterialColorValues;

	void ApplyMaterialColorValues(USkeletalMeshComponent* SkeletalMeshComponent) const;

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<UTexture*> CustomMaterialTextureValues;

	void ApplyMaterialTextureValues(USkeletalMeshComponent* SkeletalMeshComponent) const;

	void ApplyAllMaterialData(USkeletalMeshComponent* SkeletalMeshComponent) const;
private:
	mutable TMap<int32, UMaterialInstanceDynamic*> MIDMap;
	UMaterialInstanceDynamic* GetMID(USkeletalMeshComponent* SkeletalMeshComponent, int32 Idx) const;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "CustomCharacterFunctionLibrary.MakeCustomCharacterRuntimeDataRef"))
struct XD_CUSTOMCHARACTER_API FCustomCharacterRuntimeDataRef
{
	GENERATED_BODY()
public:
	FCustomCharacterRuntimeDataRef()
		:CustomCharacterRuntimeDataPtr(nullptr)
	{}

	FCustomCharacterRuntimeDataRef(FCustomCharacterRuntimeData& Data)
		:CustomCharacterRuntimeDataPtr(&Data)
	{}

	FCustomCharacterRuntimeData* operator->() { return CustomCharacterRuntimeDataPtr; }
	const FCustomCharacterRuntimeData* operator->() const { return CustomCharacterRuntimeDataPtr; }
	constexpr explicit operator bool() const noexcept { return CustomCharacterRuntimeDataPtr ? CustomCharacterRuntimeDataPtr->CustomConfig ? true : false : false; }

	FCustomCharacterRuntimeData* CustomCharacterRuntimeDataPtr;
};

UCLASS()
class XD_CUSTOMCHARACTER_API UCustomCharacterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static float GetCustomSkeletonValue(const FCustomCharacterRuntimeDataRef& Data, int32 Idx) { return Data ? Data->GetCustomSkeletonValue(Idx) : 0.f; }

	UFUNCTION(BlueprintCallable, Category = "角色|定制")
	static void SetCustomSkeletonValue(UPARAM(Ref)FCustomCharacterRuntimeDataRef& Data, int32 Idx, float InValue) { if (Data) Data->SetCustomSkeletonValue(Idx, InValue); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomSkeletonEntry GetCustomSkeletonConfig(const FCustomCharacterRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->SkeletonData[Idx] : FCustomSkeletonEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomMorphEntry GetCustomMorphConfig(const FCustomCharacterRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->MorphData[Idx] : FCustomMorphEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制", meta = (NativeMakeFunc))
	static FCustomCharacterRuntimeDataRef MakeCustomCharacterRuntimeDataRef(UPARAM(Ref) FCustomCharacterRuntimeData& Data) { return FCustomCharacterRuntimeDataRef(Data); }
};
