// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LPGameMode.generated.h"

class ULoadScreenSaveGame;
class USaveGame;
class UMVVM_LoadSlot;
class UCharacterClassInfo;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class LOSTPLACE_API ALPGameMode : public AGameMode
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly, Category="角色类默认值")
	TObjectPtr<UCharacterClassInfo> CharacterClassInfo;

	
	UPROPERTY(EditDefaultsOnly, Category="能力信息")
	TObjectPtr<UAbilityInfo> AbilityInfo;
	
	/**
	 * 创建新存档
	 * @param LoadSlot 需要保存的视图模型示例
	 * @param SlotIndex 存档索引
	 */
	void SaveSlotData(const UMVVM_LoadSlot* LoadSlot, int32 SlotIndex) const;

	/**
	 * 获取保存的存档
	 * @param SlotName 存档名称（每个存档名称固定）
	 * @param SlotIndex 存档索引
	 * @return 
	 */
	ULoadScreenSaveGame* GetSaveSlotData(const FString& SlotName, int32 SlotIndex) const;

	/**
	 * 删除存档
	 * @param SlotName 需要删除存档对应的视图模型实例名称
	 * @param SlotIndex 存档索引
	 */
	static void DeleteSlotData(const FString& SlotName, int32 SlotIndex);

	//获取到当前游戏进行中所使用的存档数据
	ULoadScreenSaveGame* RetrieveInGameSaveData() const;

	/**
	 * 保存游戏中的进度
	 * @param SaveObject 需要保存的数据
	 */
	void SaveInGameProgressData(ULoadScreenSaveGame* SaveObject);

	//保存关卡中的状态到当前存档中
	void SaveWorldState(UWorld* World) const;
	//从存档中加载当前关卡的状态
	void LoadWorldState(UWorld* World) const;

	void TravelToMap(const UMVVM_LoadSlot* Slot);

	//存档使用的数据结构
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<USaveGame> LoadScreenSaveGameClass;

	//初始地图名称
	UPROPERTY(EditDefaultsOnly)
	FString DefaultMapName;

	//游戏初始地图
	UPROPERTY(EditDefaultsOnly)
	TSoftObjectPtr<UWorld> DefaultMap;
	
	//地图名称和地图的映射
	UPROPERTY(EditDefaultsOnly)
	TMap<FString, TSoftObjectPtr<UWorld>> Maps;

	//角色切换关卡后默认生成位置的PlayerStart的标签
	UPROPERTY(EditDefaultsOnly)
	FName DefaultPlayerStartTag;

	//覆写父类的选择PlayerStart函数，修改为可以通过Tag获取生成位置
	virtual AActor* ChoosePlayerStart_Implementation(AController* Player) override;

protected:
	virtual void BeginPlay() override;

};
