// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPProjectileSpell.h"
#include "LPFireBolt.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPFireBolt : public ULPProjectileSpell
{
	GENERATED_BODY()
public:
		
	virtual FString GetDescription(int32 Level) override; //获取投射技能描述
	virtual FString GetNextLevelDescription(int32 Level) override; //获取投射技能下一等级描述


};
