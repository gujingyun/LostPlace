// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Passive/PassiveNiagaraComponent.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "Interface/CombatInterface.h"


// Sets default values for this component's properties
UPassiveNiagaraComponent::UPassiveNiagaraComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UPassiveNiagaraComponent::BeginPlay()
{
	Super::BeginPlay();
	if(UAbilitySystemComponentBase* LPASC = Cast<UAbilitySystemComponentBase>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetOwner())))
	{
		LPASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
	}
	else if(ICombatInterface* CombatInterface = Cast<ICombatInterface>(GetOwner()))
	{
		//AddWeakLambda 这种绑定方式的主要好处是，当绑定的对象被销毁时，委托不会保持对象的引用，从而避免悬空指针问题和内存泄漏。
		CombatInterface->GetOnASCRegisteredDelegate().AddWeakLambda(this,[this](UAbilitySystemComponent* InASC)
		{
			if(UAbilitySystemComponentBase* LPASC = Cast<UAbilitySystemComponentBase>(InASC))
			{
				LPASC->ActivatePassiveEffect.AddUObject(this, &UPassiveNiagaraComponent::OnPassiveActivate);
			}
		});
	}
}

void UPassiveNiagaraComponent::OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate)
{
	//判断技能标签是否一致
	if(AbilityTag.MatchesTagExact(PassiveSpellTag))
	{
		//判断是否需要激活
		if(bActivate)
		{
			//不需要重复激活
			if(!IsActive()) Activate();
		}
		else
		{
			Deactivate();
		}
	}
}


