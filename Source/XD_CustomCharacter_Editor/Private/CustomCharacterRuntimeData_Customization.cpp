// Fill out your copyright notice in the Description page of Project Settings.

#include "CustomCharacterRuntimeData_Customization.h"
#include <DetailWidgetRow.h>
#include <Widgets/Text/STextBlock.h>
#include <Widgets/Input/SNumericEntryBox.h>
#include <IDetailChildrenBuilder.h>
#include <IDetailGroup.h>
#include <IDetailPropertyRow.h>
#include <Widgets/Input/SButton.h>
#include <Engine/Texture.h>
#include <PropertyCustomizationHelpers.h>
#include <IPropertyTypeCustomization.h>
#include <IPropertyUtilities.h>

#include "XD_CustomSkeletalConfig.h"

#define LOCTEXT_NAMESPACE "CustomCharacterRuntimeData_Customization"

namespace CustomCharacterPropertyCustomize
{
	UObject* GetOuter(const TSharedRef<IPropertyHandle>& PropertyHandle)
	{
		TArray<UObject*> Outers;
		PropertyHandle->GetOuterObjects(Outers);
		return Outers.Num() > 0 ? Outers[0] : nullptr;
	}

	template<typename ValueType>
	ValueType* Value(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		if (UObject* Outer = GetOuter(PropertyHandle.ToSharedRef()))
		{
			return reinterpret_cast<ValueType*>(PropertyHandle->GetValueBaseAddress(reinterpret_cast<uint8*>(Outer)));
		}
		return nullptr;
	}

	void StructBuilderDrawPropertys(class IDetailChildrenBuilder& StructBuilder, const TSharedRef<IPropertyHandle>& PropertyHandle, const TArray<FName>& ExcludePropertyNames, UStruct* StopShowType = nullptr)
	{
		uint32 NumChildren = 0;
		PropertyHandle->GetNumChildren(NumChildren);
		for (uint32 ChildIndex = 0; ChildIndex < NumChildren; ++ChildIndex)
		{
			const TSharedRef<IPropertyHandle> ChildHandle = PropertyHandle->GetChildHandle(ChildIndex).ToSharedRef();

			FProperty* ChildProperty = ChildHandle->GetProperty();
			UStruct* OwnerStrcut = ChildProperty->GetOwnerStruct();
			if (StopShowType == nullptr || !OwnerStrcut->IsChildOf(StopShowType))
			{
				if (!ExcludePropertyNames.Contains(*ChildProperty->GetNameCPP()))
				{
					StructBuilder.AddProperty(ChildHandle);
				}
			}
		}
	}

	template<typename ValueType>
	ValueType GetValue(const TSharedPtr<IPropertyHandle>& PropertyHandle)
	{
		if (UObject* Outer = GetOuter(PropertyHandle.ToSharedRef()))
		{
			if (ValueType* Res = reinterpret_cast<ValueType*>(PropertyHandle->GetValueBaseAddress(reinterpret_cast<uint8*>(Outer))))
			{
				return *Res;
			}
		}
		return {};
	}

	template<typename Type>
	void SetValue(const TSharedPtr<IPropertyHandle>& PropertyHandle, const Type& Value, bool NotifyChange = true)
	{
		if (NotifyChange)
		{
			PropertyHandle->NotifyPreChange();
		}
		if (Type* Target = reinterpret_cast<Type*>(PropertyHandle->GetValueBaseAddress(reinterpret_cast<uint8*>(GetOuter(PropertyHandle.ToSharedRef())))))
		{
			*Target = Value;
			if (NotifyChange)
			{
				PropertyHandle->NotifyPostChange(EPropertyValueSetFlags::DefaultFlags);
			}
		}
	}
}


void FCustomCharacterRuntimeData_Customization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& CustomizationUtils)
{
	TSharedPtr<IPropertyUtilities> PropertyUtils = CustomizationUtils.GetPropertyUtilities();

	TSharedPtr<IPropertyHandle> CustomConfig_PropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FXD_CustomSkeletalRuntimeData, CustomConfig));

	FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
	CustomCharacterRuntimeData.SyncConfigData(CustomCharacterRuntimeData.CustomConfig);
	Config = CustomCharacterRuntimeData.CustomConfig;
	CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData, false);

	if (CustomConfig_PropertyHandle)
	{
		CustomConfig_PropertyHandle->SetOnPropertyValueChanged(FSimpleDelegate::CreateLambda([=]()
			{
				FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
				CustomCharacterRuntimeData.SyncConfigData(Config.Get());
				Config = CustomCharacterRuntimeData.CustomConfig;
				CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);

				PropertyUtils->ForceRefresh();
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

	FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
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
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
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
								if (FXD_CustomSkeletalRuntimeData* Value = CustomCharacterPropertyCustomize::Value<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle))
								{
									if (Idx < Value->CustomSkeletonValues.Num())
									{
										Value->SetCustomSkeletonValue(Idx, NewValue);
									}
								}
							})
						.OnValueCommitted_Lambda([=](float NewValue, ETextCommit::Type CommitType)
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								CustomCharacterRuntimeData.SetCustomSkeletonValue(Idx, NewValue);
								CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
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
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomSkeletonValues.Num())
								{
									CustomCharacterRuntimeData.SetCustomSkeletonValue(Idx, CustomCharacterRuntimeData.CustomConfig->SkeletonData[Idx].DefaultValue);
								}
								CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								return FReply::Handled();
							})
						.Visibility_Lambda([=]()
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomSkeletonValues.Num())
								{
									return CustomCharacterRuntimeData.GetCustomSkeletonValue(Idx) != CustomCharacterRuntimeData.CustomConfig->SkeletonData[Idx].DefaultValue ? EVisibility::Visible : EVisibility::Hidden;
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
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
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
								if (FXD_CustomSkeletalRuntimeData* Value = CustomCharacterPropertyCustomize::Value<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle))
								{
									if (Idx < Value->CustomMorphValues.Num())
									{
										Value->SetCustomMorphValue(Idx, NewValue, nullptr);
									}
								}
							})
						.OnValueCommitted_Lambda([=](float NewValue, ETextCommit::Type CommitType)
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								CustomCharacterRuntimeData.SetCustomMorphValue(Idx, NewValue, nullptr);
								CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
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
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMorphValues.Num())
								{
									CustomCharacterRuntimeData.SetCustomMorphValue(Idx, CustomCharacterRuntimeData.CustomConfig->MorphData[Idx].DefaultValue, nullptr);
								}
								CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								return FReply::Handled();
							})
						.Visibility_Lambda([=]()
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMorphValues.Num())
								{
									return CustomCharacterRuntimeData.GetCustomMorphValue(Idx) != CustomCharacterRuntimeData.CustomConfig->MorphData[Idx].DefaultValue ? EVisibility::Visible : EVisibility::Hidden;
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
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
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
								if (FXD_CustomSkeletalRuntimeData* Value = CustomCharacterPropertyCustomize::Value<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle))
								{
									if (Idx < Value->CustomMaterialFloatValues.Num())
									{
										Value->CustomMaterialFloatValues[Idx] = NewValue;
									}
								}
							})
						.OnValueCommitted_Lambda([=](float NewValue, ETextCommit::Type CommitType)
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx] = NewValue;
								CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
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
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialFloatValues.Num())
								{
									CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx] = CustomCharacterRuntimeData.CustomConfig->MaterialFloatData[Idx].DefaultValue;
								}
								CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								return FReply::Handled();
							})
						.Visibility_Lambda([=]()
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialFloatValues.Num())
								{
									return CustomCharacterRuntimeData.CustomMaterialFloatValues[Idx] != CustomCharacterRuntimeData.CustomConfig->MaterialFloatData[Idx].DefaultValue ? EVisibility::Visible : EVisibility::Hidden;
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
			TSharedRef<IPropertyHandleArray> CustomMaterialColorValues_Handle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FXD_CustomSkeletalRuntimeData, CustomMaterialColorValues))->AsArray().ToSharedRef();
			uint32 NumElements;
			CustomMaterialColorValues_Handle->GetNumElements(NumElements);
			if (int32(NumElements) > Idx)
			{
				//TODO 还是会显示Array存在的拖拽功能，直接自己写吧
				IDetailPropertyRow& DetailPropertyRow = GroupMap[*Entry.Category.ToString()]->AddPropertyRow(CustomMaterialColorValues_Handle->GetElement(Idx));
				DetailPropertyRow.DisplayName(Entry.DisplayName);
				DetailPropertyRow.OverrideResetToDefault(FResetToDefaultOverride::Create(FIsResetToDefaultVisible::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
					{
						FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
						if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialColorValues.Num())
						{
							return CustomCharacterRuntimeData.CustomMaterialColorValues[Idx] != CustomCharacterRuntimeData.CustomConfig->MaterialColorData[Idx].DefaultColor;
						}
						else
						{
							return false;
						}
					}),
					FResetToDefaultHandler::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
					{
						FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
						if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialColorValues.Num())
						{
							CustomCharacterRuntimeData.CustomMaterialColorValues[Idx] = CustomCharacterRuntimeData.CustomConfig->MaterialColorData[Idx].DefaultColor;
						}
						CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
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
			TSharedRef<IPropertyHandleArray> CustomMaterialTextureValues_Handle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FXD_CustomSkeletalRuntimeData, CustomMaterialTextureValues))->AsArray().ToSharedRef();
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
									FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
									UCustomMaterialTextureConfig* TextureConfig = CustomCharacterRuntimeData.CustomConfig ? CustomCharacterRuntimeData.CustomConfig->MaterialTextureData[Idx].TextureConfig : nullptr;
									bool NeedShow = TextureConfig && TextureConfig->ReplaceTextures.ContainsByPredicate([&](UTexture* Texture) { return Texture ? Texture->GetFName() == AssetData.AssetName : false; });
									return !NeedShow;
								})
							.CustomResetToDefault(FResetToDefaultOverride::Create(FIsResetToDefaultVisible::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
							{
								FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
								if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialTextureValues.Num())
								{
									return CustomCharacterRuntimeData.CustomMaterialTextureValues[Idx] != CustomCharacterRuntimeData.CustomConfig->MaterialTextureData[Idx].DefaultTexture;
								}
								else
								{
									return false;
								}
							}),
							FResetToDefaultHandler::CreateLambda([=](TSharedPtr<IPropertyHandle> PropertyHandle)
								{
									FXD_CustomSkeletalRuntimeData CustomCharacterRuntimeData = CustomCharacterPropertyCustomize::GetValue<FXD_CustomSkeletalRuntimeData>(StructPropertyHandle);
									if (CustomCharacterRuntimeData.CustomConfig && Idx < CustomCharacterRuntimeData.CustomMaterialTextureValues.Num())
									{
										CustomCharacterRuntimeData.CustomMaterialTextureValues[Idx] = CustomCharacterRuntimeData.CustomConfig->MaterialTextureData[Idx].DefaultTexture;
									}
									CustomCharacterPropertyCustomize::SetValue(StructPropertyHandle, CustomCharacterRuntimeData);
								})))
						]
					];
			}
 		}
	}
}

void FCustomSkeletonBoneData_Customization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		.MinDesiredWidth(200.f)
		[
			([&]()
				{
					TSharedPtr<IPropertyHandle> BoneName_PropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCustomSkeletonBoneData, BoneName));
					return SNew(SHorizontalBox)
						+SHorizontalBox::Slot()
						.AutoWidth()
						.Padding(2.f, 0.f)
						[
							BoneName_PropertyHandle->CreatePropertyNameWidget()
						]
						+SHorizontalBox::Slot()
						.Padding(4.f, 0.f)
						[
							BoneName_PropertyHandle->CreatePropertyValueWidget()
						];
				}())
		];
}

void FCustomSkeletonBoneData_Customization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	CustomCharacterPropertyCustomize::StructBuilderDrawPropertys(StructBuilder, StructPropertyHandle, { GET_MEMBER_NAME_CHECKED(FCustomSkeletonBoneData, BoneName) });
}

void FCustomCharacter_HorizontalShow_Customization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		.MinDesiredWidth(200.f)
		[
			([&]()
				{
					TSharedRef<SHorizontalBox> HorizontalBox = SNew(SHorizontalBox);
					uint32 ChildNum;
					if (StructPropertyHandle->GetNumChildren(ChildNum) != FPropertyAccess::Fail)
					{
						for (uint32 Idx = 0; Idx < ChildNum; ++Idx)
						{
							TSharedPtr<IPropertyHandle> PropertyHandle = StructPropertyHandle->GetChildHandle(Idx);
							HorizontalBox->AddSlot()
								.AutoWidth()
								.Padding(4.f, 0.f)
								[
									PropertyHandle->CreatePropertyNameWidget()
								];
							HorizontalBox->AddSlot()
								.AutoWidth()
								.Padding(4.f, 0.f)
								[
									PropertyHandle->CreatePropertyValueWidget()
								];
						}
					}
					return HorizontalBox;
				}())
		];
}

void FCustomCharacter_ShowNameAndCategory_Customization::CustomizeHeader(TSharedRef<class IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
	.ValueContent()
		.MinDesiredWidth(300.f)
		[
			([&]()
				{
					TSharedPtr<IPropertyHandle> DisplayName_PropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCustomCharacterEntryBase, DisplayName));
					TSharedPtr<IPropertyHandle> Category_PropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FCustomCharacterEntryBase, Category));
					return SNew(SHorizontalBox)
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.FillWidth(0.15f)
						.Padding(4.f, 0.f)
						[
							DisplayName_PropertyHandle->CreatePropertyNameWidget()
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.FillWidth(0.35f)
						.Padding(4.f, 0.f)
						[
							DisplayName_PropertyHandle->CreatePropertyValueWidget()
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.FillWidth(0.15f)
						.Padding(4.f, 0.f)
						[
							Category_PropertyHandle->CreatePropertyNameWidget()
						]
						+ SHorizontalBox::Slot()
						.AutoWidth()
						.FillWidth(0.35f)
						.Padding(4.f, 0.f)
						[
							Category_PropertyHandle->CreatePropertyValueWidget()
						];
				}())
		];
}

void FCustomCharacter_ShowNameAndCategory_Customization::CustomizeChildren(TSharedRef<class IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	CustomCharacterPropertyCustomize::StructBuilderDrawPropertys(StructBuilder, StructPropertyHandle, { GET_MEMBER_NAME_CHECKED(FCustomCharacterEntryBase, DisplayName), GET_MEMBER_NAME_CHECKED(FCustomCharacterEntryBase, Category) });
}

#undef LOCTEXT_NAMESPACE
