// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "XD_CustomCharacter_Editor.h"
#include <PropertyEditorModule.h>
#include <AssetToolsModule.h>
#include <IAssetTools.h>

#include "CustomCharacterRuntimeData_Customization.h"
#include "XD_CustomCharacterFactory.h"

#define LOCTEXT_NAMESPACE "FXD_CustomCharacter_EditorModule"

struct FXD_CustomSkeletalRuntimeData;
struct FCustomSkeletonBoneData;
struct FCustomMaterialFloatData;
struct FCustomMaterialTextureData;
struct FCustomMaterialColorData;
struct FCustomSkeletonEntry;
struct FCustomMorphEntry;
struct FCustomMaterialFloatEntry;
struct FCustomMaterialColorEntry;
struct FCustomMaterialTextureEntry;

uint32 FXD_CustomCharacter_EditorModule::CustomCharacter_AssetCategory;

void FXD_CustomCharacter_EditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
	{
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("XD_CustomSkeletalRuntimeData"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacterRuntimeData_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomSkeletonBoneData"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomSkeletonBoneData_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMaterialFloatData"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_HorizontalShow_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMaterialTextureData"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_HorizontalShow_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMaterialColorData"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_HorizontalShow_Customization::MakeInstance));

		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomSkeletonEntry"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_ShowNameAndCategory_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMorphEntry"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_ShowNameAndCategory_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMaterialFloatEntry"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_ShowNameAndCategory_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMaterialColorEntry"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_ShowNameAndCategory_Customization::MakeInstance));
		PropertyModule.RegisterCustomPropertyTypeLayout(TEXT("CustomMaterialTextureEntry"), FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FCustomCharacter_ShowNameAndCategory_Customization::MakeInstance));
	}
	
	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	{
		CustomCharacter_AssetCategory = AssetTools.RegisterAdvancedAssetCategory(TEXT("角色定制"), LOCTEXT("角色定制", "角色定制"));
		AssetTypeActions_CustomCharacterConfig = MakeShareable(new FAssetTypeActions_CustomCharacterConfig());

		AssetTools.RegisterAssetTypeActions(AssetTypeActions_CustomCharacterConfig.ToSharedRef());

		AssetTypeActions_CustomMaterialTextureConfig = MakeShareable(new FAssetTypeActions_CustomMaterialTextureConfig());
		AssetTools.RegisterAssetTypeActions(AssetTypeActions_CustomMaterialTextureConfig.ToSharedRef());
	}
}

void FXD_CustomCharacter_EditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.


	if (FAssetToolsModule* AssetToolsModule = FModuleManager::Get().GetModulePtr<FAssetToolsModule>("AssetTools"))
	{
		IAssetTools& AssetTools = AssetToolsModule->Get();
		AssetTools.UnregisterAssetTypeActions(AssetTypeActions_CustomCharacterConfig.ToSharedRef());
		AssetTools.UnregisterAssetTypeActions(AssetTypeActions_CustomMaterialTextureConfig.ToSharedRef());
	}

	if (FPropertyEditorModule* PropertyModulePtr = FModuleManager::LoadModulePtr<FPropertyEditorModule>("PropertyEditor"))
	{
		FPropertyEditorModule& PropertyModule = *PropertyModulePtr;

		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("XD_CustomSkeletalRuntimeData"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomSkeletonBoneData"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMaterialFloatData"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMaterialTextureData"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMaterialColorData"));

		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomSkeletonEntry"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMorphEntry"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMaterialFloatEntry"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMaterialColorEntry"));
		PropertyModule.UnregisterCustomPropertyTypeLayout(TEXT("CustomMaterialTextureEntry"));
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_CustomCharacter_EditorModule, XD_CustomCharacter_Editor)