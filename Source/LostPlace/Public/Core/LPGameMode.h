// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "LPGameMode.generated.h"

class UCharacterClassInfo;
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

};
