// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "XD_CustomCharacter_Editor.h"
#include <PropertyEditorModule.h>
#include <AssetToolsModule.h>
#include <IAssetTools.h>

#include "XD_PropertyCustomizationEx.h"
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
		RegisterCustomProperty(FXD_CustomSkeletalRuntimeData, FCustomCharacterRuntimeData_Customization);
		RegisterCustomProperty(FCustomSkeletonBoneData, FCustomSkeletonBoneData_Customization);
		RegisterCustomProperty(FCustomMaterialFloatData, FCustomCharacter_HorizontalShow_Customization);
		RegisterCustomProperty(FCustomMaterialTextureData, FCustomCharacter_HorizontalShow_Customization);
		RegisterCustomProperty(FCustomMaterialColorData, FCustomCharacter_HorizontalShow_Customization);

		RegisterCustomProperty(FCustomSkeletonEntry, FCustomCharacter_ShowNameAndCategory_Customization);
		RegisterCustomProperty(FCustomMorphEntry, FCustomCharacter_ShowNameAndCategory_Customization);
		RegisterCustomProperty(FCustomMaterialFloatEntry, FCustomCharacter_ShowNameAndCategory_Customization);
		RegisterCustomProperty(FCustomMaterialColorEntry, FCustomCharacter_ShowNameAndCategory_Customization);
		RegisterCustomProperty(FCustomMaterialTextureEntry, FCustomCharacter_ShowNameAndCategory_Customization);
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
		UnregisterCustomProperty(FXD_CustomSkeletalRuntimeData);
		UnregisterCustomProperty(FCustomSkeletonBoneData);
		UnregisterCustomProperty(FCustomMaterialFloatData);
		UnregisterCustomProperty(FCustomMaterialTextureData);
		UnregisterCustomProperty(FCustomMaterialColorData);

		UnregisterCustomProperty(FCustomSkeletonEntry);
		UnregisterCustomProperty(FCustomMorphEntry);
		UnregisterCustomProperty(FCustomMaterialFloatEntry);
		UnregisterCustomProperty(FCustomMaterialColorEntry);
		UnregisterCustomProperty(FCustomMaterialTextureEntry);
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_CustomCharacter_EditorModule, XD_CustomCharacter_Editor)