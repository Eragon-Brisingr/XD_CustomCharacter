// Fill out your copyright notice in the Description page of Project Settings.


#include "XD_CustomCharacterFactory.h"
#include <AssetTypeCategories.h>

#include "CustomSkeletonConfig.h"
#include "XD_CustomCharacter_Editor.h"

#define LOCTEXT_NAMESPACE "XD_CustomCharacterFactory"

UXD_CustomCharacterConfigFactory::UXD_CustomCharacterConfigFactory()
{
	SupportedClass = UCustomCharacterConfig::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UXD_CustomCharacterConfigFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(UCustomCharacterConfig::StaticClass()));
	return NewObject<UCustomCharacterConfig>(InParent, Class, Name, Flags);
}

FText UXD_CustomCharacterConfigFactory::GetDisplayName() const
{
	return LOCTEXT("创建角色定制配置", "创建角色定制配置");
}

FText FAssetTypeActions_CustomCharacterConfig::GetName() const
{
	return LOCTEXT("角色定制配置", "角色定制配置");
}

UClass* FAssetTypeActions_CustomCharacterConfig::GetSupportedClass() const
{
	return UCustomCharacterConfig::StaticClass();
}

FColor FAssetTypeActions_CustomCharacterConfig::GetTypeColor() const
{
	return FColor::Green;
}

uint32 FAssetTypeActions_CustomCharacterConfig::GetCategories()
{
	return FXD_CustomCharacter_EditorModule::CustomCharacter_AssetCategory;
}

UXD_CustomMaterialTextureConfigFactory::UXD_CustomMaterialTextureConfigFactory()
{
	SupportedClass = UCustomMaterialTextureConfig::StaticClass();
	bCreateNew = true;
	bEditAfterNew = true;
}

UObject* UXD_CustomMaterialTextureConfigFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn, FName CallingContext)
{
	check(Class->IsChildOf(UCustomMaterialTextureConfig::StaticClass()));
	return NewObject<UCustomMaterialTextureConfig>(InParent, Class, Name, Flags);
}

FText UXD_CustomMaterialTextureConfigFactory::GetDisplayName() const
{
	return LOCTEXT("创建角色定制_贴图集", "创建角色定制_贴图集");
}

FText FAssetTypeActions_CustomMaterialTextureConfig::GetName() const
{
	return LOCTEXT("角色定制_贴图集", "角色定制_贴图集");
}

UClass* FAssetTypeActions_CustomMaterialTextureConfig::GetSupportedClass() const
{
	return UCustomMaterialTextureConfig::StaticClass();
}

FColor FAssetTypeActions_CustomMaterialTextureConfig::GetTypeColor() const
{
	return FColor::Green;
}

uint32 FAssetTypeActions_CustomMaterialTextureConfig::GetCategories()
{
	return FXD_CustomCharacter_EditorModule::CustomCharacter_AssetCategory;
}

#undef LOCTEXT_NAMESPACE
