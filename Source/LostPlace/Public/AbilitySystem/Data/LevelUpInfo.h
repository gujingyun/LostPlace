// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "LevelUpInfo.generated.h"
//角色升级数据结构体
USTRUCT(BlueprintType)
struct FRPGLevelUpInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0; //升到此等级所需经验值

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1; //达到此等级奖励的属性点值

	UPROPERTY(EditDefaultsOnly)
	int32 SpellPointAward = 1; //达到此等级降级的可分配技能点数
};

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULevelUpInfo : public UDataAsset
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FRPGLevelUpInfo> LevelUpInformation; //当前所有等级的升级数据

	int32 FindLevelForXP(int32 XP) const; //通过经验值值获取角色的等级
};
