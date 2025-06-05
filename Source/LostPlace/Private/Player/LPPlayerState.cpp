// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LPPlayerState.h"

#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "Net/UnrealNetwork.h"

ALPPlayerState::ALPPlayerState()
{
	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>("AttributeSet");
	
	SetNetUpdateFrequency(100.f); //每秒和服务器更新频率，使用GAS后可以设置的高一些
}

void ALPPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME(ALPPlayerState, Level);
}

UAbilitySystemComponent* ALPPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALPPlayerState::OnRep_Level(int32 OldLevel)
{
	
}
