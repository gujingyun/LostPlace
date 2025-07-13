// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadSlot::InitializeSlot() const
{
	const int32 WidgetSwitcherIndex = LoadSlotStatus.GetIntValue();
	SetWidgetSwitcherIndex.Broadcast(WidgetSwitcherIndex);
}

void UMVVM_LoadSlot::SetSlotName(const FString& InSlotName)
{
	UE_MVVM_SET_PROPERTY_VALUE(SlotName, InSlotName);
}

void UMVVM_LoadSlot::SetPlayerName(const FString& InPlayName)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerName, InPlayName);
}

void UMVVM_LoadSlot::SetMapName(const FString& InMapName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapName, InMapName);
}

void UMVVM_LoadSlot::SetMapAssetName(const FString& InMapAssetName)
{
	UE_MVVM_SET_PROPERTY_VALUE(MapAssetName, InMapAssetName);
}

void UMVVM_LoadSlot::SetPlayerStartTag(const FName& InPlayerStartTag)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerStartTag, InPlayerStartTag);
}

void UMVVM_LoadSlot::SetPlayerLevel(const int32& InPlayerLevel)
{
	UE_MVVM_SET_PROPERTY_VALUE(PlayerLevel, InPlayerLevel);
}
