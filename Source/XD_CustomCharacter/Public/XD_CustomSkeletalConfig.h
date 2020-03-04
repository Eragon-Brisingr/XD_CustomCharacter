// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Engine/DataAsset.h>
#include <Kismet/BlueprintFunctionLibrary.h>
#include "XD_CustomSkeletalConfig.generated.h"

class USkeletalMeshComponent;
class UMaterialInstanceDynamic;
class UTexture;

/**
 *
 */
USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomCharacterEntryBase
{
	GENERATED_BODY()
public:
	FCustomCharacterEntryBase() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "名称"))
	FText DisplayName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "分类"))
	FText Category;
};

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomCharacterRuntimeNumber
{
	GENERATED_BODY()
public:
	FCustomCharacterRuntimeNumber() = default;

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

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomSkeletonBoneData
{
	GENERATED_BODY()
public:
	FCustomSkeletonBoneData() = default;

	UPROPERTY(EditAnywhere, Category = "骨架定制", meta = (DisplayName = "骨骼名"))
	FName BoneName;

	UPROPERTY(EditAnywhere, Category = "骨架定制", meta = (DisplayName = "变换模式"))
	ECustomSkeletonMode Mode;

	UPROPERTY(EditAnywhere, Category = "骨架定制", meta = (DisplayName = "轴向比例"))
	FVector ApplyAxis;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomSkeletonEntry : public FCustomCharacterEntryBase
{
	GENERATED_BODY()
public:
	FCustomSkeletonEntry() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "影响骨架"))
	TArray<FCustomSkeletonBoneData> BoneDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最大值"))
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最小值"))
	float MinValue = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认值"))
	float DefaultValue;

	FText GetBonesDesc() const;

	FCustomCharacterRuntimeNumber ToRuntimeData() const
	{
		FCustomCharacterRuntimeNumber Entry;
		Entry.SetValue(DefaultValue, MinValue, MaxValue);
		return Entry;
	}
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMorphEntry : public FCustomCharacterEntryBase
{
	GENERATED_BODY()
public:
	FCustomMorphEntry() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "混合变形名称"))
	TArray<FName> MorphTargetNames;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最大值"))
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最小值"))
	float MinValue = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认值"))
	float DefaultValue;

	FText GetMorphsDesc() const;
};

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomMaterialDataBase
{
	GENERATED_BODY()
public:
	FCustomMaterialDataBase() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "材质插槽名"))
	FName SlotName;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "参数名"))
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

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomMaterialFloatData : public FCustomMaterialDataBase
{
	GENERATED_BODY()
public:
	FCustomMaterialFloatData() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "材质通道类型"))
	ECustomMaterialFloatType CustomMaterialFloatType;
};

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomMaterialColorData : public FCustomMaterialDataBase
{
	GENERATED_BODY()
public:
	FCustomMaterialColorData() = default;
};

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FCustomMaterialTextureData : public FCustomMaterialDataBase
{
	GENERATED_BODY()
public:
	FCustomMaterialTextureData() = default;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialFloatEntry : public FCustomCharacterEntryBase
{
	GENERATED_BODY()
public:
	FCustomMaterialFloatEntry() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "材质定制数值"))
	TArray<FCustomMaterialFloatData> CustomMaterialFloatDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最大值"))
	float MaxValue = 1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "最小值"))
	float MinValue = -1.f;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认值"))
	float DefaultValue;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialColorEntry : public FCustomCharacterEntryBase
{
	GENERATED_BODY()
public:
	FCustomMaterialColorEntry() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "材质定制颜色"))
	TArray<FCustomMaterialColorData> CustomMaterialColorDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "默认颜色"))
	FLinearColor DefaultColor;
};

UCLASS(meta = (DisplayName = "角色定制_贴图集"))
class XD_CUSTOMCHARACTER_API UCustomMaterialTextureConfig : public UObject
{
	GENERATED_BODY()
public:
	UCustomMaterialTextureConfig() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (DisplayName = "贴图"))
	TArray<UTexture*> ReplaceTextures;
};

USTRUCT(BlueprintType)
struct XD_CUSTOMCHARACTER_API FCustomMaterialTextureEntry : public FCustomCharacterEntryBase
{
	GENERATED_BODY()
public:
	FCustomMaterialTextureEntry() = default;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制", meta = (DisplayName = "材质定制贴图"))
	TArray<FCustomMaterialTextureData> CustomMaterialTextureDatas;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制", meta = (DisplayName = "默认贴图"))
	UTexture* DefaultTexture;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "材质定制", meta = (DisplayName = "贴图集"))
	UCustomMaterialTextureConfig* TextureConfig;
};

UCLASS(meta = (DisplayName = "角色定制配置"))
class XD_CUSTOMCHARACTER_API UXD_CustomSkeletalConfig : public UObject
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制", meta = (DisplayName = "骨骼定制"))
	TArray<FCustomSkeletonEntry> SkeletonData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制", meta = (DisplayName = "混合变形定制"))
	TArray<FCustomMorphEntry> MorphData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制", meta = (DisplayName = "材质数值定制"))
	TArray<FCustomMaterialFloatEntry> MaterialFloatData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制", meta = (DisplayName = "材质颜色定制"))
	TArray<FCustomMaterialColorEntry> MaterialColorData;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "角色定制", meta = (DisplayName = "材质贴图定制"))
	TArray<FCustomMaterialTextureEntry> MaterialTextureData;

	TArray<FText> GetAllCategoryNames() const;
};

USTRUCT(BlueprintType, BlueprintInternalUseOnly)
struct XD_CUSTOMCHARACTER_API FXD_CustomSkeletalRuntimeData
{
	GENERATED_BODY()
public:
	FXD_CustomSkeletalRuntimeData() = default;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "角色定制")
	UXD_CustomSkeletalConfig* CustomConfig;

	void SetCustomCharacterRuntimeData(const FXD_CustomSkeletalRuntimeData& NewCustomCharacterRuntimeData);
	void SetCustomConfig(UXD_CustomSkeletalConfig* NewCustomConfig);
	void SyncConfigData(UXD_CustomSkeletalConfig* OldCustomConfig);

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<FCustomCharacterRuntimeNumber> CustomSkeletonValues;

	float GetCustomSkeletonValue(int32 Idx) const;
	float GetCustomSkeletonValueScaled(int32 Idx) const;
	void SetCustomSkeletonValue(int32 Idx, float InValue);

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<FCustomCharacterRuntimeNumber> CustomMorphValues;

	float GetCustomMorphValue(int32 Idx) const;
	void SetCustomMorphValue(int32 Idx, float InValue, USkeletalMeshComponent* SkeletalMeshComponent);
	void ApplyMorphTarget(USkeletalMeshComponent* SkeletalMeshComponent) const;

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<float> CustomMaterialFloatValues;

	float GetCustomMaterialFloatValue(int32 Idx) const { return CustomMaterialFloatValues[Idx]; }
	void SetCustomMaterialFloatValue(int32 Idx, float InValue, USkeletalMeshComponent* SkeletalMeshComponent);
	void ApplyMaterialFloatValues(USkeletalMeshComponent* SkeletalMeshComponent) const;

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<FLinearColor> CustomMaterialColorValues;

	FLinearColor GetCustomMaterialColorValue(int32 Idx) const { return CustomMaterialColorValues[Idx]; }
	void SetCustomMaterialColorValue(int32 Idx, const FLinearColor& InValue, USkeletalMeshComponent* SkeletalMeshComponent);
	void ApplyMaterialColorValues(USkeletalMeshComponent* SkeletalMeshComponent) const;

	UPROPERTY(EditAnywhere, Category = "角色定制", EditFixedSize, SaveGame)
	TArray<UTexture*> CustomMaterialTextureValues;

	UTexture* GetCustomMaterialTextureValue(int32 Idx) const { return CustomMaterialTextureValues[Idx]; }
	void SetCustomMaterialTextureValue(int32 Idx, UTexture* InValue, USkeletalMeshComponent* SkeletalMeshComponent);
	void ApplyMaterialTextureValues(USkeletalMeshComponent* SkeletalMeshComponent) const;

	void ApplyAllMaterialData(USkeletalMeshComponent* SkeletalMeshComponent) const;
private:
	struct FLinearColorTypeData
	{
		int32 IdxR = INDEX_NONE;
		int32 IdxG = INDEX_NONE;
		int32 IdxB = INDEX_NONE;
		int32 IdxA = INDEX_NONE;
	};

	UMaterialInstanceDynamic* GetMID(USkeletalMeshComponent* SkeletalMeshComponent, const FName& SlotName) const;
};

USTRUCT(BlueprintType, meta = (HasNativeMake = "CustomCharacterFunctionLibrary.MakeCustomSkeletalRuntimeDataRef"))
struct XD_CUSTOMCHARACTER_API FXD_CustomSkeletalRuntimeDataRef
{
	GENERATED_BODY()
public:
	FXD_CustomSkeletalRuntimeDataRef()
		:CustomCharacterRuntimeDataPtr(nullptr)
	{}

	FXD_CustomSkeletalRuntimeDataRef(FXD_CustomSkeletalRuntimeData& Data)
		:CustomCharacterRuntimeDataPtr(&Data)
	{}

	FXD_CustomSkeletalRuntimeData* operator->() { return CustomCharacterRuntimeDataPtr; }
	const FXD_CustomSkeletalRuntimeData* operator->() const { return CustomCharacterRuntimeDataPtr; }
	constexpr explicit operator bool() const noexcept { return CustomCharacterRuntimeDataPtr ? CustomCharacterRuntimeDataPtr->CustomConfig ? true : false : false; }

	FXD_CustomSkeletalRuntimeData* CustomCharacterRuntimeDataPtr;
};

UCLASS()
class XD_CUSTOMCHARACTER_API UCustomCharacterFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static UXD_CustomSkeletalConfig* GetCustomConfig(const FXD_CustomSkeletalRuntimeDataRef& Data) { return Data ? Data->CustomConfig : nullptr; }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static float GetCustomSkeletonValue(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->GetCustomSkeletonValue(Idx) : 0.f; }

	UFUNCTION(BlueprintCallable, Category = "角色|定制")
	static void SetCustomSkeletonValue(UPARAM(Ref)FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx, float InValue) { if (Data) Data->SetCustomSkeletonValue(Idx, InValue); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomSkeletonEntry GetCustomSkeletonConfig(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->SkeletonData[Idx] : FCustomSkeletonEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static float GetCustomMorphValue(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->GetCustomMorphValue(Idx) : 0.f; }

	UFUNCTION(BlueprintCallable, Category = "角色|定制")
	static void SetCustomMorphValue(UPARAM(Ref)FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx, float InValue, USkeletalMeshComponent* SkeletalMeshComponent) { if (Data) Data->SetCustomMorphValue(Idx, InValue, SkeletalMeshComponent); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomMorphEntry GetCustomMorphConfig(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->MorphData[Idx] : FCustomMorphEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static float GetCustomMaterialFloatValue(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->GetCustomMaterialFloatValue(Idx) : 0.f; }

	UFUNCTION(BlueprintCallable, Category = "角色|定制")
	static void SetCustomMaterialFloatValue(UPARAM(Ref)FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx, float InValue, USkeletalMeshComponent* SkeletalMeshComponent) { if (Data) Data->SetCustomMaterialFloatValue(Idx, InValue, SkeletalMeshComponent); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomMaterialFloatEntry GetCustomMaterialFloatConfig(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->MaterialFloatData[Idx] : FCustomMaterialFloatEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FLinearColor GetCustomMaterialColorValue(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->GetCustomMaterialColorValue(Idx) : FLinearColor::White; }

	UFUNCTION(BlueprintCallable, Category = "角色|定制")
	static void SetCustomMaterialColorValue(UPARAM(Ref)FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx, const FLinearColor& InValue, USkeletalMeshComponent* SkeletalMeshComponent) { if (Data) Data->SetCustomMaterialColorValue(Idx, InValue, SkeletalMeshComponent); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomMaterialColorEntry GetCustomMaterialColorConfig(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->MaterialColorData[Idx] : FCustomMaterialColorEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static UTexture* GetCustomMaterialTextureValue(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->GetCustomMaterialTextureValue(Idx) : nullptr; }

	UFUNCTION(BlueprintCallable, Category = "角色|定制")
	static void SetCustomMaterialTextureValue(UPARAM(Ref)FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx, UTexture* InValue, USkeletalMeshComponent* SkeletalMeshComponent) { if (Data) Data->SetCustomMaterialTextureValue(Idx, InValue, SkeletalMeshComponent); }

	UFUNCTION(BlueprintPure, Category = "角色|定制")
	static FCustomMaterialTextureEntry GetCustomMaterialTextureConfig(const FXD_CustomSkeletalRuntimeDataRef& Data, int32 Idx) { return Data ? Data->CustomConfig->MaterialTextureData[Idx] : FCustomMaterialTextureEntry(); }

	UFUNCTION(BlueprintPure, Category = "角色|定制", meta = (NativeMakeFunc, CompactNodeTitle = "ToRef", BlueprintAutocast, BlueprintThreadSafe))
	static FXD_CustomSkeletalRuntimeDataRef MakeCustomSkeletalRuntimeDataRef(const FXD_CustomSkeletalRuntimeData& Data) { return FXD_CustomSkeletalRuntimeDataRef(const_cast<FXD_CustomSkeletalRuntimeData&>(Data)); }
};
