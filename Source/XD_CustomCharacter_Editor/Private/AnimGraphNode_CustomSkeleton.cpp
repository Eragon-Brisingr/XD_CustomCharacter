// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimGraphNode_CustomSkeleton.h"

#define LOCTEXT_NAMESPACE "CustomSkeleton"

FText UAnimGraphNode_CustomSkeleton::GetTooltipText() const
{
	return LOCTEXT("CustomSkeleton", "CustomSkeleton");
}

FLinearColor UAnimGraphNode_CustomSkeleton::GetNodeTitleColor() const
{
	return FLinearColor(0.2f, 0.2f, 0.8f);
}

FText UAnimGraphNode_CustomSkeleton::GetNodeTitle(ENodeTitleType::Type TitleType) const
{
	return LOCTEXT("CustomSkeleton", "CustomSkeleton");
}

FString UAnimGraphNode_CustomSkeleton::GetNodeCategory() const
{
	return TEXT("AnimNode");
}

#undef LOCTEXT_NAMESPACE
