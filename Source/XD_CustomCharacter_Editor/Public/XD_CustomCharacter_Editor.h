// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include <Modules/ModuleManager.h>

class FAssetTypeActions_CustomCharacterConfig;
class FAssetTypeActions_CustomMaterialTextureConfig;

class FXD_CustomCharacter_EditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	static uint32 CustomCharacter_AssetCategory;
private:
	TSharedPtr<FAssetTypeActions_CustomCharacterConfig> AssetTypeActions_CustomCharacterConfig;
	TSharedPtr<FAssetTypeActions_CustomMaterialTextureConfig> AssetTypeActions_CustomMaterialTextureConfig;
};
