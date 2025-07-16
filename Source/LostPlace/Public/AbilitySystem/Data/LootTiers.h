// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LootTiers.generated.h"
USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	//战利品在场景中的显示效果
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	//战利品生成几率
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	float ChanceToSpawn = 0.f;

	//物品生成的最大数量
	UPROPERTY(EditAnywhere, Category="LootTiers|Spawning")
	int32 MaxNumberToSpawn = 0.f;

	//修改物品生成等级，false则使用敌人等级
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULootTiers : public UDataAsset
{
	GENERATED_BODY()
	
public:

	//获取需要生成的战利品数据
	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GetLootItems();

	UPROPERTY(EditDefaultsOnly, Category="LootTiers|Spawning")
	TArray<FLootItem> LootItems;
};
