// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "AbilitySystem/Data/AttributeInfo.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "LPGameplayTags.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "Player/LPPlayerState.h"

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	const UAttributeSetBase* AS = Cast<UAttributeSetBase>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
			[this,Pair](const FOnAttributeChangeData& Data)
			{
				BroadcastAttributeInfo(Pair.Key, Pair.Value());
			}
		);
	}
	ALPPlayerState* PLPlayerState = CastChecked<ALPPlayerState>(PlayerState);
	PLPlayerState->OnAttributePointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		AttributePointsChangedDelegate.Broadcast(Points);
	});
	PLPlayerState->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPointsChangedDelegate.Broadcast(Points);
	});
}

void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	const UAttributeSetBase* AS = Cast<UAttributeSetBase>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	ALPPlayerState* PLPlayerState = CastChecked<ALPPlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(PLPlayerState->GetAttributePoints());
	SpellPointsChangedDelegate.Broadcast(PLPlayerState->GetSpellPoints());
}

void UAttributeMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UAbilitySystemComponentBase* ASC = CastChecked<UAbilitySystemComponentBase>(AbilitySystemComponent);
	ASC->UpgradeAttribute(AttributeTag);
}

void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const
{
	FLPAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
