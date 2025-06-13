// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/SpellMenuWidgetController.h"

#include "LPGameplayTags.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Player/LPPlayerState.h"

void USpellMenuWidgetController::BroadcastInitialValues()
{
	BroadcastAbilityInfo();
	SpellPointsChangedDelegate.Broadcast(GetLPPS()->GetSpellPoints());
}

void USpellMenuWidgetController::BindCallbacksToDependencies()
{

	GetLPASC()->AbilityStatusChanged.AddLambda([this](const FGameplayTag& AbilityTag, const FGameplayTag& StatusTag,int32 NewLevel)
	{
		if (SelectedAbility.Ability.MatchesTag(AbilityTag))
		{
			SelectedAbility.Status = StatusTag;
			bool bEnableSpendPoints = false;
			bool bEnableEquip = false;
			ShouldEnableButtons(StatusTag,CurrentSpellPoints,bEnableSpendPoints,bEnableEquip);
			FString Description;
			FString NextLevelDescription;
			GetLPASC()->GetDescriptionByAbilityTag(AbilityTag,Description,NextLevelDescription);
			SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,Description,NextLevelDescription);
		}
		if(AbilityInfo)
		{
			FLPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);//获取到技能数据
			Info.StatusTag = StatusTag;
			AbilityInfoDelegate.Broadcast(Info);
		}
	});
	GetLPPS()->OnSpellPointsChangedDelegate.AddLambda([this](const int32 Points)
	{
		SpellPointsChangedDelegate.Broadcast(Points);
		CurrentSpellPoints = Points;
		bool bEnableSpendPoints = false;
		bool bEnableEquip = false;
		ShouldEnableButtons(SelectedAbility.Status,CurrentSpellPoints,bEnableSpendPoints,bEnableEquip);
		FString Description;
		FString NextLevelDescription;
		GetLPASC()->GetDescriptionByAbilityTag(SelectedAbility.Ability,Description,NextLevelDescription);
		SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,Description,NextLevelDescription);
	});
}

void USpellMenuWidgetController::SpellGlobeSelected(const FGameplayTag& AbilityTag)
{
	const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
	const int32 SpellPoints = GetLPPS()->GetSpellPoints();
	FGameplayTag AbilityStatus;

	const bool bTagIsValid = AbilityTag.IsValid();
	const bool bTagNone = AbilityTag.MatchesTag(GameplayTags.Abilities_None);
	FGameplayAbilitySpec* AbilitySpec = GetLPASC()->GetSpecFromAbilityTag(AbilityTag);
	const bool bSpecValid = AbilitySpec!=nullptr;
	if (!bTagIsValid||bTagNone||!bSpecValid)
	{
		AbilityStatus = GameplayTags.Abilities_Status_Locked;
	}else
	{
		AbilityStatus = GetLPASC()->GetStatusFromSpec(*AbilitySpec);
	}
	SelectedAbility.Ability = AbilityTag;
	SelectedAbility.Status = AbilityStatus;
	
	bool bEnableSpendPoints = false;
	bool bEnableEquip = false;
	ShouldEnableButtons(AbilityStatus,SpellPoints,bEnableSpendPoints,bEnableEquip);

	FString Description;
	FString NextLevelDescription;
	GetLPASC()->GetDescriptionByAbilityTag(AbilityTag,Description,NextLevelDescription);
	SpellGlobeSelectedDelegate.Broadcast(bEnableSpendPoints,bEnableEquip,Description,NextLevelDescription);
}

void USpellMenuWidgetController::SpendPointButtonPressed()
{
	if (GetLPASC())
	{
		GetLPASC()->ServerSpendSpellPoint(SelectedAbility.Ability);
	}
}

void USpellMenuWidgetController::GlobeDeselect()
{
	const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
	SelectedAbility.Ability = GameplayTags.Abilities_None;
	SelectedAbility.Status = GameplayTags.Abilities_Status_Locked;
	SpellGlobeSelectedDelegate.Broadcast(false,false,FString(), FString());
}

void USpellMenuWidgetController::ShouldEnableButtons(const FGameplayTag& AbilityStatus, int32 SpellPoints,
                                                     bool& bShouldEnableSpellPoints, bool& bShouldEnableEquipButton)
{
	const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
	bShouldEnableEquipButton = false;
	bShouldEnableSpellPoints = false;
	if (AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Equipped))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPoints = true;
		}
	}
	else if (AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Eligible))
	{
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPoints = true;
		}
	}
	else if (AbilityStatus.MatchesTag(GameplayTags.Abilities_Status_Unlocked))
	{
		bShouldEnableEquipButton = true;
		if (SpellPoints > 0)
		{
			bShouldEnableSpellPoints = true;
		}
	}
}
