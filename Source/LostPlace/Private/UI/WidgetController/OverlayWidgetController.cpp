// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/OverlayWidgetController.h"

#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "LostPlace/LPLogChannels.h"
#include "Player/LPPlayerState.h"

void UOverlayWidgetController::BroadcastInitialValues()
{
	const UAttributeSetBase* AttributeSetBase = CastChecked<UAttributeSetBase>(AttributeSet);

	OnHealthChanged.Broadcast(AttributeSetBase->GetHealth());
	OnMaxHealthChanged.Broadcast(AttributeSetBase->GetMaxHealth());
	OnMPChanged.Broadcast(AttributeSetBase->GetMP());
	OnMaxMPChanged.Broadcast(AttributeSetBase->GetMaxMP());
}

void UOverlayWidgetController::BindCallbacksToDependencies()
{
	ALPPlayerState* LPPlayerState = CastChecked<ALPPlayerState>(PlayerState);
	LPPlayerState->OnXPChangedDelegate.AddUObject(this, &UOverlayWidgetController::OnXPChanged);

	//绑定等级相关回调
	LPPlayerState->OnLevelChangedDelegate.AddLambda([this](int32 NewLevel)
	{
		OnPlayerLevelChangeDelegate.Broadcast(NewLevel);
	});

	
	const UAttributeSetBase* AttributeSetBase = CastChecked<UAttributeSetBase>(AttributeSet);


	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetMaxHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxHealthChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetMPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMPChanged.Broadcast(Data.NewValue);
		}
	);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		AttributeSetBase->GetMaxMPAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnMaxMPChanged.Broadcast(Data.NewValue);
		}
	);
	if (TObjectPtr<UAbilitySystemComponentBase> ASCBase = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent))
	{
		if(ASCBase->bStartupAbilitiesGiven)
		{
			//如果执行到此处时，技能的初始化工作已经完成，则直接调用初始化回调
			OnInitializeStartupAbilities(ASCBase);
		}
		else
		{
			//如果执行到此处，技能初始化还未完成，将通过绑定委托，监听广播的形式触发初始化完成回调
			ASCBase->AbilityGivenDelegate.AddUObject(this, &ThisClass::OnInitializeStartupAbilities);
		}
		ASCBase->EffectAssetTags.AddLambda([this](const FGameplayTagContainer& TagsContainer)
		{
			for (const FGameplayTag& Tag : TagsContainer)
			{
					
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		});
	}
	

	
}

void UOverlayWidgetController::OnInitializeStartupAbilities(
	UAbilitySystemComponentBase* AbilitySystemComponentBase) const
{
	if(!AbilitySystemComponentBase->bStartupAbilitiesGiven) return; //判断当前技能初始化是否完成，触发回调时都已经完成

	//创建单播委托
	FForEachAbility BroadcastDelegate;
	//委托绑定回调匿名函数，委托广播时将会触发函数内部逻辑
	BroadcastDelegate.BindLambda([this](const FGameplayAbilitySpec& AbilitySpec)
	{
		//通过静态函数获取到技能实例的技能标签，并通过标签获取到技能数据
		FLPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(UAbilitySystemComponentBase::GetAbilityTagFromSpec(AbilitySpec));
		//获取到技能的输入标签
		Info.InputTag = UAbilitySystemComponentBase::GetInputTagFromSpec(AbilitySpec);
		//广播技能数据
		AbilityInfoDelegate.Broadcast(Info); 
	});
	//遍历技能并触发委托回调
	AbilitySystemComponentBase->ForEachAbility(BroadcastDelegate);
}

void UOverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	const ALPPlayerState* LPPlayerState = CastChecked<ALPPlayerState>(PlayerState);
	const ULevelUpInfo* LevelUpInfo = LPPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("无法查询到等级相关数据，请查看PlayerState是否设置了对应的数据"));
	const int32 Level =  LevelUpInfo->FindLevelForXP(NewXP); //根据当前经验值查询对应的等级
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num(); //获取当前最大等级
	if(Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement; //获取对应等级的升级经验值
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level-1].LevelUpRequirement;
		const float XPPercent = static_cast<float>(NewXP - PreviousLevelUpRequirement) / (LevelUpRequirement - PreviousLevelUpRequirement);
		OnXPPercentChangedDelegate.Broadcast(XPPercent);
	}
	
}




