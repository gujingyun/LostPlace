// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "Core/LoadScreenSaveGame.h"
#include "MVVM_LoadSlot.generated.h"
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetWidgetSwitcherIndex, int32, WidgetSwitecherIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEnableSelectSlotButton, bool, bEnable);
/**
 * 
 */
UCLASS()
class LOSTPLACE_API UMVVM_LoadSlot : public UMVVMViewModelBase
{
	GENERATED_BODY()
public:
	
	//切换存档显示的用户控件的委托
	UPROPERTY(BlueprintAssignable)
	FSetWidgetSwitcherIndex SetWidgetSwitcherIndex;
	
	UPROPERTY(BlueprintAssignable)
	FEnableSelectSlotButton EnableSelectSlotButton;
	
	void InitializeSlot() const;
	//当前视图模型的索引，对应存档的索引
	UPROPERTY()
	int32 SlotIndex;

	//当前进入加载存档界面时，此存档应该显示的用户控件界面。
	UPROPERTY()
	TEnumAsByte<ESaveSlotStatus> LoadSlotStatus;
	
	void SetSlotName(const FString& InSlotName);
	FString GetSlotName() const { return SlotName; };
	//设置存档的状态
	void SetPlayerName(const FString& InPlayName);
	FString GetPlayerName() const { return PlayerName; };

	void SetMapName(const FString& InMapName);
	FString GetMapName() const { return MapName; };

	void SetPlayerStartTag(const FName& InPlayerStartTag);
	FName GetPlayerStartTag() const { return PlayerStartTag; };

	void SetPlayerLevel(const int32& InPlayerLevel);
	int32 GetPlayerLevel() const { return PlayerLevel; };
private:

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString PlayerName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString MapName;
	
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	int32 PlayerLevel;
	
	//用户控件的名称
	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FString SlotName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, Setter, Getter, meta=(AllowPrivateAccess))
	FName PlayerStartTag;

};
