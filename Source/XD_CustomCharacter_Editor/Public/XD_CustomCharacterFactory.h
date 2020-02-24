// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Factories/Factory.h>
#include <AssetTypeActions_Base.h>
#include "XD_CustomCharacterFactory.generated.h"

/**
 * 
 */
UCLASS()
class XD_CUSTOMCHARACTER_EDITOR_API UXD_CustomCharacterConfigFactory : public UFactory
{
	GENERATED_BODY()
public:
	UXD_CustomCharacterConfigFactory();

 	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;
 
 	FText GetDisplayName() const override;
};

class FAssetTypeActions_CustomCharacterConfig : public FAssetTypeActions_Base
{
	using Super = FAssetTypeActions_Base;

	// Inherited via FAssetTypeActions_Base
	FText GetName() const override;
	UClass* GetSupportedClass() const override;
	FColor GetTypeColor() const override;
	uint32 GetCategories() override;
};

UCLASS()
class XD_CUSTOMCHARACTER_EDITOR_API UXD_CustomMaterialTextureConfigFactory : public UFactory
{
	GENERATED_BODY()
public:
	UXD_CustomMaterialTextureConfigFactory();

	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext) override;

	FText GetDisplayName() const override;
};

class FAssetTypeActions_CustomMaterialTextureConfig : public FAssetTypeActions_Base
{
	using Super = FAssetTypeActions_Base;

	// Inherited via FAssetTypeActions_Base
	FText GetName() const override;
	UClass* GetSupportedClass() const override;
	FColor GetTypeColor() const override;
	uint32 GetCategories() override;
};

