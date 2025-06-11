// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "LPWidgetController.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "OverlayWidgetController.generated.h"

class UAbilityInfo;
class ULPUserWidget;
class UAbilitySystemComponentBase;
USTRUCT(BlueprintType)
struct FUIWidgetRow : public FTableRowBase
{
	GENERATED_BODY();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag MessageTag = FGameplayTag();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FText Message = FText();

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<ULPUserWidget> MessageWidget;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UTexture2D* Image = nullptr;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnAttributeChangedSignature,float, NewValue);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMessageWidgetRowSignature,FUIWidgetRow, Row);



/**
 * 屏幕覆盖用户控件控制器层基类，继承与用户控件控制器层，添加了四个广播，分别是生命值、最大生命值、魔法值、最大魔法值。
 */
UCLASS(BlueprintType, Blueprintable)
class LOSTPLACE_API UOverlayWidgetController : public ULPWidgetController
{
	GENERATED_BODY()
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxHealthChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMPChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnAttributeChangedSignature OnMaxMPChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Message")
	FMessageWidgetRowSignature MessageWidgetRowDelegate;


	UPROPERTY(BlueprintAssignable, Category="GAS|XP")
	FOnAttributeChangedSignature OnXPPercentChangedDelegate; //经验条百分比变动回调

	UPROPERTY(BlueprintAssignable, Category="GAS|Level")
	FOnPlayerStateChangedSignature OnPlayerLevelChangeDelegate; //等级变动回调


protected:
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category="Widget Data")
	TObjectPtr<UDataTable> MessageWidgetDataTable;
	

	template<typename T>
	T* GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag);
	
	void OnXPChanged(int32 NewXP); //经验变动后的回调

};

template <typename T>
T* UOverlayWidgetController::GetDataTableRowByTag(UDataTable* DataTable, const FGameplayTag& Tag)
{
	return DataTable->FindRow<T>(Tag.GetTagName(),TEXT(""));
}
