// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "XD_PropertyCustomizationEx.h"

/**
 *
 */
class XD_CUSTOMCHARACTER_EDITOR_API FCustomCharacterRuntimeData_Customization : public IPropertyTypeCustomizationMakeInstanceable<FCustomCharacterRuntimeData_Customization>
{
public:
	void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

private:
	TWeakObjectPtr<class UCustomCharacterConfig> Config;
};

class FCustomSkeletonBoneData_Customization : public IPropertyTypeCustomizationMakeInstanceable<FCustomSkeletonBoneData_Customization>
{
public:
	void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};

class FCustomCharacter_HorizontalShow_Customization : public IPropertyTypeCustomizationMakeInstanceable<FCustomCharacter_HorizontalShow_Customization>
{
public:
	void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override {}
};

class FCustomCharacter_ShowNameAndCategory_Customization : public IPropertyTypeCustomizationMakeInstanceable<FCustomCharacter_ShowNameAndCategory_Customization>
{
public:
	void CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	void CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
};
