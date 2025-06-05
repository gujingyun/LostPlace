// Fill out your copyright notice in the Description page of Project Settings.


#include "LPAssetManager.h"

#include "LPGameplayTags.h"

ULPAssetManager& ULPAssetManager::Get()
{
	check(GEngine && GEngine->AssetManager);
	return *CastChecked<ULPAssetManager>(GEngine->AssetManager);
}

void ULPAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	FLPGameplayTags::InitializeNativeGameplayTags();
	//	5.4版本之后不需要初始化全局数据了，引擎自动初始化了
	// UAbilitySystemGlobals::Get().InitGlobalData();
}
