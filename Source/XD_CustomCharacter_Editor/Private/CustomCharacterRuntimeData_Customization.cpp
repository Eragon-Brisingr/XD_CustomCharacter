// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCharacterRuntimeData_Customization.h"
#include "DetailWidgetRow.h"
#include "CustomSkeletonConfig.h"
#include "STextBlock.h"
#include "SNumericEntryBox.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailGroup.h"
#include "IDetailPropertyRow.h"
#include "SButton.h"
#include "XD_PropertyCustomizationEx.h"
#include "Engine/Texture.h"
#include "PropertyCustomizationHelpers.h"

#define LOCTEXT_NAMESPACE "CustomCharacterRuntimeData_Customization"

void FCustomCharacterRuntimeData_Customization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	TSharedPtr<IPropertyHandle> CustomConfig_PropertyHandle = FPropertyCustomizeHelper::GetPropertyHandleByName(StructPropertyHandle, GET_MEMBER_NAME_CHECKED(FCustomCharacterRuntimeData, CustomConfig));

	FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
	CustomCharacterRuntimeData.SyncConfigSize();
	FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData, false);

	if (CustomConfig_PropertyHandle)
	{
		CustomConfig_PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([StructPropertyHandle]()
			{
				FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
				if (CustomCharacterRuntimeData.CustomConfig)
				{
					TArray<FCustomSkeletonEntry>& Data = CustomCharacterRuntimeData.CustomConfig->SkeletonData;
					int32 Length = Data.Num();
					CustomCharacterRuntimeData.CustomSkeletonValues.SetNum(Length);
					for (int32 Idx = 0; Idx < Length; ++Idx)
					{
						CustomCharacterRuntimeData.CustomSkeletonValues[Idx] = Data[Idx].ToRuntimeData();
					}
				}
				else
				{
					CustomCharacterRuntimeData.CustomSkeletonValues.SetNum(0);
				}
				FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
			}));

		HeaderRow.NameContent()
			[
				StructPropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			.MinDesiredWidth(200.f)
			[
				CustomConfig_PropertyHandle->CreatePropertyValueWidget()
			];
	}
	else
	{
		HeaderRow.NameContent()
			[
				StructPropertyHandle->CreatePropertyNameWidget()
			]
			.ValueContent()
			[
				SNew(STextBlock)
					.Text(FText::FromName(CustomCharacterRuntimeData.CustomConfig ? CustomCharacterRuntimeData.CustomConfig->GetFName() : NAME_None))
			];
	}
}

namespace FCustomCharacterWidget
{
	class SResetToDefaultButton : public SCompoundWidget
	{
	public:
		SLATE_BEGIN_ARGS(SResetToDefaultButton)
			:_ToolTipText(LOCTEXT("ResetToDefaultToolTip", "Reset to Default"))
		{}
		SLATE_ARGUMENT(FText, ToolTipText)
		SLATE_ARGUMENT(EVisibility, Visibility)
		SLATE_EVENT( FOnClicked, OnClicked )

		SLATE_END_ARGS()


		void Construct(const FArguments& InArgs)
		{
			// Indicator for a value that differs from default. Also offers the option to reset to default.
			ChildSlot
			[
				SNew(SButton)
				.IsFocusable(false)
				.ToolTipText(InArgs._ToolTipText)
				.ButtonStyle(FEditorStyle::Get(), "NoBorder")
				.ContentPadding(0)
				.Visibility(InArgs._Visibility)
				.OnClicked(InArgs._OnClicked)
				.Content()
				[
					SNew(SImage)
					.Image( FEditorStyle::GetBrush("PropertyWindow.DiffersFromDefault") )
				]
			];
		}
	};
}

void FCustomCharacterRuntimeData_Customization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	//TODO 提供返回至默认值的按钮 SResetToDefaultPropertyEditor

	FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
	if (CustomCharacterRuntimeData.CustomConfig)
	{
		TMap<FName, IDetailGroup*> GroupMap;
		for (const FText& Category : CustomCharacterRuntimeData.CustomConfig->GetAllCategoryNames())
		{
			FName CategoryName = *Category.ToString();
			IDetailGroup*& DetailGroup = GroupMap.FindOrAdd(CategoryName);
			if (!DetailGroup)
			{
				DetailGroup = &StructBuilder.AddGroup(CategoryName, Category);
			}
		}

		TArray<FCustomSkeletonEntry>& SkeletonData = CustomCharacterRuntimeData.CustomConfig->SkeletonData;
		for (int32 Idx = 0; Idx < SkeletonData.Num(); ++Idx)
		{
			const FCustomSkeletonEntry& Entry = SkeletonData[Idx];
			GroupMap[*Entry.Category.ToString()]->AddWidgetRow().NameContent()
				[
					SNew(STextBlock)
					.Text(Entry.DisplayName)
					.ToolTipText(Entry.GetBonesDesc())
				]
			.ValueContent()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(Entry.MinValue)
						.MinSliderValue(Entry.MinValue)
						.MaxValue(Entry.MaxValue)
						.MaxSliderValue(Entry.MaxValue)
						.AllowSpin(true)
						.Value_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (Idx < CustomCharacterRuntimeData.CustomSkeletonValues.Num())
								{
									return CustomCharacterRuntimeData.GetCustomSkeletonValue(Idx);
								}
								else
								{
									return 0.f;
								}
							})
						.OnValueChanged_Lambda([=](float NewValue)
							{
								if (FCustomCharacterRuntimeData* Value = FPropertyCustomizeHelper::Value<FCustomCharacterRuntimeData>(StructPropertyHandle))
								{
									if (Idx < Value->CustomSkeletonValues.Num())
									{
										Value->SetCustomSkeletonValue(Idx, NewValue);
									}
								}
							})
						.OnValueCommitted_Lambda([=](float NewValue, ETextCommit::Type CommitType)
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								CustomCharacterRuntimeData.SetCustomSkeletonValue(Idx, NewValue);
								FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
							})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Padding(FMargin(8, 0, 2, 0))
					[
						SNew(FCustomCharacterWidget::SResetToDefaultButton)
						.OnClicked_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomSkeletonValues.Num())
								{
									CustomCharacterRuntimeData.SetCustomSkeletonValue(Idx, CustomCharacterRuntimeData.CustomConfig->SkeletonData[Idx].DefalutValue);
								}
								FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								return FReply::Handled();
							})
						.Visibility_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomSkeletonValues.Num())
								{
									return CustomCharacterRuntimeData.GetCustomSkeletonValue(Idx) != CustomCharacterRuntimeData.CustomConfig->SkeletonData[Idx].DefalutValue ? EVisibility::Visible : EVisibility::Hidden;
								}
								else
								{
									return EVisibility::Hidden;
								}
							})
					]
				];
		}

		TArray<FCustomMorphEntry>& MorphData = CustomCharacterRuntimeData.CustomConfig->MorphData;
		for (int32 Idx = 0; Idx < MorphData.Num(); ++Idx)
		{
			const FCustomMorphEntry& Entry = MorphData[Idx];
			GroupMap[*Entry.Category.ToString()]->AddWidgetRow().NameContent()
				[
					SNew(STextBlock)
					.Text(Entry.DisplayName)
					.ToolTipText(Entry.GetMorphsDesc())
				]
			.ValueContent()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(Entry.MinValue)
						.MinSliderValue(Entry.MinValue)
						.MaxValue(Entry.MaxValue)
						.MaxSliderValue(Entry.MaxValue)
						.AllowSpin(true)
						.Value_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (Idx < CustomCharacterRuntimeData.CustomMorphValues.Num())
								{
									return CustomCharacterRuntimeData.GetCustomMorphValue(Idx);
								}
								else
								{
									return 0.f;
								}
							})
						.OnValueChanged_Lambda([=](float NewValue)
							{
								if (FCustomCharacterRuntimeData* Value = FPropertyCustomizeHelper::Value<FCustomCharacterRuntimeData>(StructPropertyHandle))
								{
									if (Idx < Value->CustomMorphValues.Num())
									{
										Value->SetCustomMorphValue(Idx, NewValue);
									}
								}
							})
						.OnValueCommitted_Lambda([=](float NewValue, ETextCommit::Type CommitType)
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								CustomCharacterRuntimeData.SetCustomMorphValue(Idx, NewValue);
								FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
							})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Padding(FMargin(8, 0, 2, 0))
					[
						SNew(FCustomCharacterWidget::SResetToDefaultButton)
						.OnClicked_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMorphValues.Num())
								{
									CustomCharacterRuntimeData.SetCustomMorphValue(Idx, CustomCharacterRuntimeData.CustomConfig->MorphData[Idx].DefalutValue);
								}
								FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								return FReply::Handled();
							})
						.Visibility_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMorphValues.Num())
								{
									return CustomCharacterRuntimeData.GetCustomMorphValue(Idx) != CustomCharacterRuntimeData.CustomConfig->MorphData[Idx].DefalutValue ? EVisibility::Visible : EVisibility::Hidden;
								}
								else
								{
									return EVisibility::Hidden;
								}
							})
					]
				];
		}

		TArray<FCustomMaterialFloatEntry>& MaterialFloatData = CustomCharacterRuntimeData.CustomConfig->MaterialFloatData;
		for (int32 Idx = 0; Idx < MaterialFloatData.Num(); ++Idx)
		{
			const FCustomMaterialFloatEntry& Entry = MaterialFloatData[Idx];
			GroupMap[*Entry.Category.ToString()]->AddWidgetRow().NameContent()
				[
					SNew(STextBlock)
					.Text(Entry.DisplayName)
				]
			.ValueContent()
				[
					SNew(SHorizontalBox)
					+SHorizontalBox::Slot()
					[
						SNew(SNumericEntryBox<float>)
						.MinValue(Entry.MinValue)
						.MinSliderValue(Entry.MinValue)
						.MaxValue(Entry.MaxValue)
						.MaxSliderValue(Entry.MaxValue)
						.AllowSpin(true)
						.Value_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (Idx < CustomCharacterRuntimeData.CustomMaterialFloatValues.Num())
								{
									return CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx];
								}
								else
								{
									return 0.f;
								}
							})
						.OnValueChanged_Lambda([=](float NewValue)
							{
								if (FCustomCharacterRuntimeData* Value = FPropertyCustomizeHelper::Value<FCustomCharacterRuntimeData>(StructPropertyHandle))
								{
									if (Idx < Value->CustomMaterialFloatValues.Num())
									{
										Value->CustomMaterialFloatValues[Idx] = NewValue;
									}
								}
							})
						.OnValueCommitted_Lambda([=](float NewValue, ETextCommit::Type CommitType)
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx] = NewValue;
								FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
							})
					]
					+ SHorizontalBox::Slot()
					.AutoWidth()
					.VAlign(VAlign_Center)
					.HAlign(HAlign_Center)
					.Padding(FMargin(8, 0, 2, 0))
					[
						SNew(FCustomCharacterWidget::SResetToDefaultButton)
						.OnClicked_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialFloatValues.Num())
								{
									CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx] = CustomCharacterRuntimeData.CustomConfig->MaterialFloatData[Idx].DefalutValue;
								}
								FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								return FReply::Handled();
							})
						.Visibility_Lambda([=]()
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialFloatValues.Num())
								{
									return CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx] != CustomCharacterRuntimeData.CustomConfig->MaterialFloatData[Idx].DefalutValue ? EVisibility::Visible : EVisibility::Hidden;
								}
								else
								{
									return EVisibility::Hidden;
								}
							})
					]
				];
		}

 		TArray<FCustomMaterialColorEntry>& MaterialColorData = CustomCharacterRuntimeData.CustomConfig->MaterialColorData;
 		for (int32 Idx = 0; Idx < MaterialColorData.Num(); ++Idx)
 		{
 			const FCustomMaterialColorEntry& Entry = MaterialColorData[Idx];
			TSharedRef<IPropertyHandleArray> CustomMaterialColorValues_Handle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCustomCharacterRuntimeData, CustomMaterialColorValues))->AsArray().ToSharedRef();
			uint32 NumElements;
			CustomMaterialColorValues_Handle->GetNumElements(NumElements);
			if (int32(NumElements) > Idx)
			{
				//TODO 还是会显示Array存在的拖拽功能，直接自己写吧
				IDetailPropertyRow& DetailPropertyRow = GroupMap[*Entry.Category.ToString()]->AddPropertyRow(CustomMaterialColorValues_Handle->GetElement(Idx));
				DetailPropertyRow.DisplayName(Entry.DisplayName);
				DetailPropertyRow.OverrideResetToDefault(FResetToDefaultOverride::Create(FIsResetToDefaultVisible::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
					{
						FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
						if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialColorValues.Num())
						{
							return CustomCharacterRuntimeData.CustomMaterialColorValues[Idx] != CustomCharacterRuntimeData.CustomConfig->MaterialColorData[Idx].DefalutColor;
						}
						else
						{
							return false;
						}
					}),
					FResetToDefaultHandler::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
					{
						FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
						if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialColorValues.Num())
						{
							CustomCharacterRuntimeData.CustomMaterialColorValues[Idx] = CustomCharacterRuntimeData.CustomConfig->MaterialColorData[Idx].DefalutColor;
						}
						FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
					})));
				DetailPropertyRow.GetPropertyHandle()->SetOnChildPropertyValuePreChange(FSimpleDelegate::CreateLambda([=]()
					{
						StructPropertyHandle->NotifyPreChange();
					}));
				DetailPropertyRow.GetPropertyHandle()->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([=]()
					{
						StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
					}));
			}
 		}
 
 		TArray<FCustomMaterialTextureEntry>& MaterialTextureData = CustomCharacterRuntimeData.CustomConfig->MaterialTextureData;
 		for (int32 Idx = 0; Idx < MaterialTextureData.Num(); ++Idx)
 		{
			//TODO 从预制的地方选择
 			const FCustomMaterialTextureEntry& Entry = MaterialTextureData[Idx];
			TSharedRef<IPropertyHandleArray> CustomMaterialTextureValues_Handle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCustomCharacterRuntimeData, CustomMaterialTextureValues))->AsArray().ToSharedRef();
			uint32 NumElements;
			CustomMaterialTextureValues_Handle->GetNumElements(NumElements);
			if (int32(NumElements) > Idx)
			{
				TSharedRef<IPropertyHandle> Element = CustomMaterialTextureValues_Handle->GetElement(Idx);
 				Element->SetOnChildPropertyValuePreChange(FSimpleDelegate::CreateLambda([=]()
 					{
 						StructPropertyHandle->NotifyPreChange();
 					}));
 				Element->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([=]()
 					{
 						StructPropertyHandle->NotifyPostChange(EPropertyChangeType::ValueSet);
 					}));

				GroupMap[*Entry.Category.ToString()]->AddWidgetRow().NameContent()
					[
						SNew(STextBlock)
						.Text(Entry.DisplayName)
					]
					.ValueContent()
					.MinDesiredWidth(200.f)
					[
						SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						[
							SNew(SObjectPropertyEntryBox)
							.AllowedClass(UTexture::StaticClass())
							.PropertyHandle(Element)
							.OnShouldFilterAsset_Lambda([=](const FAssetData& AssetData)
								{
									FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
									UCustomMaterialTextureConfig* TextureConfig = CustomCharacterRuntimeData.CustomConfig ? CustomCharacterRuntimeData.CustomConfig->MaterialTextureData[Idx].TextureConfig : nullptr;
									bool NeedShow = TextureConfig && TextureConfig->ReplaceTextures.ContainsByPredicate([&](UTexture* Texture) { return Texture ? Texture->GetFName() == AssetData.AssetName : false; });
									return !NeedShow;
								})
							.CustomResetToDefault(FResetToDefaultOverride::Create(FIsResetToDefaultVisible::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
							{
								FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialTextureValues.Num())
								{
									return CustomCharacterRuntimeData.CustomMaterialTextureValues[Idx] != CustomCharacterRuntimeData.CustomConfig->MaterialTextureData[Idx].DefalutTexture;
								}
								else
								{
									return false;
								}
							}),
							FResetToDefaultHandler::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
								{
									FCustomCharacterRuntimeData CustomCharacterRuntimeData = FPropertyCustomizeHelper::GetValue<FCustomCharacterRuntimeData>(StructPropertyHandle);
									if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialTextureValues.Num())
									{
										CustomCharacterRuntimeData.CustomMaterialTextureValues[Idx] = CustomCharacterRuntimeData.CustomConfig->MaterialTextureData[Idx].DefalutTexture;
									}
									FPropertyCustomizeHelper::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								})))
						]
					];
			}
 		}
	}
}

#undef LOCTEXT_NAMESPACE
