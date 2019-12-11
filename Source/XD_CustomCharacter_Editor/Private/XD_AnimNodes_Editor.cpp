// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "XD_CustomCharacter_Editor.h"
#include <PropertyEditorModule.h>
#include "XD_PropertyCustomizationEx.h"

#define LOCTEXT_NAMESPACE "FXD_CustomCharacter_EditorModule"

void FXD_CustomCharacter_EditorModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module

	FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");

	RegisterCustomProperty(struct FCustomCharacterRuntimeData, FCustomCharacterRuntimeData_Customization);
}

void FXD_CustomCharacter_EditorModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FXD_CustomCharacter_EditorModule, XD_CustomCharacter_Editor)