// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LOSTPLACE_API USpellMenuWidgetController : public ULPWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override; //广播初始化的值
	virtual void BindCallbacksToDependencies() override; //绑定数值变动后回调的广播
};
