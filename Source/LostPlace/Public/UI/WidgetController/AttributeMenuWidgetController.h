// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPWidgetController.h"
#include "GameplayTagContainer.h"
#include "AttributeMenuWidgetController.generated.h"


struct FGameplayAttribute;
class UAttributeInfo;
struct FLPAttributeInfo;
struct FGameplayTag;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttibuteInfoSignature, const FLPAttributeInfo&, Info);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LOSTPLACE_API UAttributeMenuWidgetController : public ULPWidgetController
{
	GENERATED_BODY()
public:

	virtual void BindCallbacksToDependencies() override;

	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")//设置BlueprintAssignable可以在蓝图作为委托绑定监听
	FAttibuteInfoSignature AttributeInfoDelegate;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStateChangedSignature AttributePointsChangedDelegate; //监听属性点的变化委托
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnPlayerStateChangedSignature SpellPointsChangedDelegate; //监听属性点的变化委托

	UFUNCTION(BlueprintCallable, Category="GAS|Attributes")
	void UpgradeAttribute(const FGameplayTag& AttributeTag); //升级属性

protected:

	UPROPERTY(EditDefaultsOnly)//EditDefaultsOnly只能在UE面板编辑
	TObjectPtr<UAttributeInfo> AttributeInfo;
private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
