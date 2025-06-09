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
	
	//定义在多人游戏中，需要在网络中复制的属性，当属性发生变化，修改将被发送到其它客户端和服务器
	DOREPLIFETIME(ALPPlayerState, Level);
	DOREPLIFETIME(ALPPlayerState, XP);
	DOREPLIFETIME(ALPPlayerState, AttributePoints);
	DOREPLIFETIME(ALPPlayerState, SpellPoints);
}

UAbilitySystemComponent* ALPPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ALPPlayerState::AddToLevel(int32 InLevel)
{
	Level += InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void ALPPlayerState::SetLevel(int32 InLevel)
{
	Level = InLevel;
	OnLevelChangedDelegate.Broadcast(Level);
}

void ALPPlayerState::AddToXP(int32 InXP)
{
	XP += InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ALPPlayerState::SetXP(int32 InXP)
{
	XP = InXP;
	OnXPChangedDelegate.Broadcast(XP);
}

void ALPPlayerState::AddToAttributePoints(int32 InAttributePoints)
{
	AttributePoints += InAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ALPPlayerState::SetAttributePoints(int32 InAttributePoints)
{
	AttributePoints = InAttributePoints;
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ALPPlayerState::AddToSpellPoints(int32 InSpellPoints)
{
	SpellPoints += InSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void ALPPlayerState::SetSpellPoints(int32 InSpellPoints)
{
	SpellPoints = InSpellPoints;
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}

void ALPPlayerState::OnRep_Level(int32 OldLevel) const
{
	OnLevelChangedDelegate.Broadcast(Level);//上面修改委托只会在服务器触发，在此处设置是在服务器更新到客户端本地后触发
}

void ALPPlayerState::OnRep_XP(int32 OldXP) const
{
	OnXPChangedDelegate.Broadcast(XP);//上面修改委托只会在服务器触发，在此处设置是在服务器更新到客户端本地后触发
}

void ALPPlayerState::OnRep_AttributePoints(int32 OldAttributePoints) const
{
	OnAttributePointsChangedDelegate.Broadcast(AttributePoints);
}

void ALPPlayerState::OnRep_SpellPoints(int32 OldSpellPoints) const
{
	OnSpellPointsChangedDelegate.Broadcast(SpellPoints);
}
