// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "LPAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static  ULPAssetManager& Get();
protected:
	virtual void StartInitialLoading() override;
};
