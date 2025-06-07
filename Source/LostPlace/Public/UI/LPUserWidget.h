// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LPUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable) //蓝图可调用
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly) //蓝图可读取，但不能修改
	TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent) //c++里不能定义，可以调用，蓝图中 无返回值可作为通知，有返回值还可以覆盖重写
	void WidgetControllerSet();
};