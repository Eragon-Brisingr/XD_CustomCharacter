// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AnimGraphNode_Base.h"
#include "AnimNode_CustomSkeleton.h"
#include "AnimGraphNode_CustomSkeleton.generated.h"

/**
 * 
 */
UCLASS()
class XD_CUSTOMCHARACTER_EDITOR_API UAnimGraphNode_CustomSkeleton : public UAnimGraphNode_Base
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, Category = Settings)
	FAnimNode_CustomSkeleton Node;

 	//~ Begin UEdGraphNode Interface.
 	FText GetTooltipText() const override;
	FLinearColor GetNodeTitleColor() const override;
 	FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	FString GetNodeCategory() const override;
 	//~ End UEdGraphNode Interface.
};
