// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPBeamSpell.h"
#include "Electrocute.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API UElectrocute : public ULPBeamSpell
{
	GENERATED_BODY()
public:
	virtual FString GetDescription(int32 Level) override; //获取投射技能描述
	virtual FString GetDescriptionAtLevel(int32 Level, const FString& Title) override;
	virtual FString GetNextLevelDescription(int32 Level) override; //获取投射技能下一等级描述
};
