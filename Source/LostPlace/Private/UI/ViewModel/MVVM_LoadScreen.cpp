// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/ViewModel/MVVM_LoadScreen.h"

#include "Core/LPGameInstance.h"
#include "Core/LPGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "UI/ViewModel/MVVM_LoadSlot.h"

void UMVVM_LoadScreen::SetWidgetName(const FString& InSlotName)
{
	if (UE_MVVM_SET_PROPERTY_VALUE(WidgetName, InSlotName))
	{
		// UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetHealthPercent); //通过宏调用其它函数的广播
	}
}

void UMVVM_LoadScreen::InitializeLoadSlots()
{
	LoadSlot_0 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_0->SetSlotName("LoadSlot_0");
	LoadSlot_0->SlotIndex = 0;
	LoadSlots.Add(0, LoadSlot_0);
	
	LoadSlot_1 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_1->SetSlotName("LoadSlot_1");
	LoadSlot_1->SlotIndex = 1;
	LoadSlots.Add(1, LoadSlot_1);
	
	LoadSlot_2 = NewObject<UMVVM_LoadSlot>(this, LoadSlotViewModelClass);
	LoadSlot_2->SetSlotName("LoadSlot_2");
	LoadSlot_2->SlotIndex = 2;
	LoadSlots.Add(2, LoadSlot_2);
}

UMVVM_LoadSlot* UMVVM_LoadScreen::GetLoadSlotViewModelByIndex(int32 Index) const
{
	return LoadSlots.FindChecked(Index);
}

void UMVVM_LoadScreen::NewSlotButtonPressed(int32 Slot, const FString& EnterName)
{
	ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this));
	LoadSlots[Slot]->SetMapName(LPGameMode->DefaultMapName); //修改MVVM上存储的角色名称
	LoadSlots[Slot]->SetPlayerName(EnterName); //修改MVVM上存储的角色名称
	LoadSlots[Slot]->SetPlayerLevel(1);
	LoadSlots[Slot]->LoadSlotStatus = Taken; //修改进入界面为加载界面
	LoadSlots[Slot]->SetPlayerStartTag(LPGameMode->DefaultPlayerStartTag); //修改MVVM上存储的出生点标签

	
	LPGameMode->SaveSlotData(LoadSlots[Slot], Slot); //保存数据
	LoadSlots[Slot]->InitializeSlot(); //调用初始化

	ULPGameInstance* LPGameInstance = Cast<ULPGameInstance>(LPGameMode->GetGameInstance());
	LPGameInstance->LoadSlotName = LoadSlots[Slot]->GetSlotName(); //保存存档名称
	LPGameInstance->LoadSlotIndex = LoadSlots[Slot]->SlotIndex; //保存存档索引
	LPGameInstance->PlayerStartTag = LPGameMode->DefaultPlayerStartTag; //保存玩家出生点标签
}

void UMVVM_LoadScreen::NewGameButtonPressed(int32 Slot)
{
	LoadSlots[Slot]->SetWidgetSwitcherIndex.Broadcast(1);
}

void UMVVM_LoadScreen::SelectSlotButtonPressed(int32 Slot)
{
	SlotSelected.Broadcast();
	for(const TTuple<int32, UMVVM_LoadSlot*> LoadSlot : LoadSlots)
	{
		LoadSlot.Value->EnableSelectSlotButton.Broadcast(LoadSlot.Key!=Slot);
	}
	SelectedLoadSlot = LoadSlots[Slot];
}

void UMVVM_LoadScreen::DeleteButtonPressed()
{
	//删除存档
	if (IsValid(SelectedLoadSlot))
	{
		ALPGameMode::DeleteSlotData(SelectedLoadSlot->GetSlotName(), SelectedLoadSlot->SlotIndex);
		//修改用户控件显示
		SelectedLoadSlot->LoadSlotStatus = Vacant; //修改为创建存档
		SelectedLoadSlot->InitializeSlot(); //修改存档显示
		SelectedLoadSlot->EnableSelectSlotButton.Broadcast(true);
	}
	

}

void UMVVM_LoadScreen::PlayButtonPressed()
{
	//获取到加载存档界面的GameMode
	ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this));
	//获取到加载存档界面的GameInstance
	ULPGameInstance* LPGameInstance = Cast<ULPGameInstance>(LPGameMode->GetGameInstance());
	LPGameInstance->PlayerStartTag = SelectedLoadSlot->GetPlayerStartTag();
	LPGameInstance->LoadSlotName = SelectedLoadSlot->GetSlotName();
	LPGameInstance->LoadSlotIndex = SelectedLoadSlot->SlotIndex;
	LPGameInstance->PlayerStartTag = SelectedLoadSlot->GetPlayerStartTag();
	if (IsValid(SelectedLoadSlot))
	{
		LPGameMode->TravelToMap(SelectedLoadSlot);
	}
}

void UMVVM_LoadScreen::LoadData()
{
	//获取到加载存档界面的GameMode
	ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this));

	//遍历映射，获取对应存档
	for(const TTuple<int32, UMVVM_LoadSlot*> Slot : LoadSlots)
	{
		ULoadScreenSaveGame* SaveGame = LPGameMode->GetSaveSlotData(Slot.Value->GetSlotName(), Slot.Key);

		//获取存档数据
		const FString PlayerName = SaveGame->PlayerName;
		const TEnumAsByte<ESaveSlotStatus> SaveSlotStatus = SaveGame->SaveSlotStatus;

		//设置存档视图模型数据
		Slot.Value->SetPlayerName(PlayerName);
		Slot.Value->LoadSlotStatus = SaveSlotStatus;
		Slot.Value->SetMapName(SaveGame->MapName);
		Slot.Value->SetPlayerStartTag(SaveGame->PlayerStartTag);
		Slot.Value->SetPlayerLevel(SaveGame->PlayerLevel);
		//调用视图模型初始化
		Slot.Value->InitializeSlot();
	}
}
