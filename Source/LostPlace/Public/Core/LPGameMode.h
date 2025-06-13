// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LPGameMode.generated.h"

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

};
