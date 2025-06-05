// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyWidgetController.h"
#include "GameplayTagContainer.h"
#include "AttributeMenuWidgetController.generated.h"


struct FGameplayAttribute;
class UAttributeInfo;
struct FLPAttributeInfo;



DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttibuteInfoSignature, const FLPAttributeInfo&, Info);
/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class LOSTPLACE_API UAttributeMenuWidgetController : public UMyWidgetController
{
	GENERATED_BODY()
public:

	virtual void BindCallbacksToDependencies() override;

	UFUNCTION(BlueprintCallable)
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")//设置BlueprintAssignable可以在蓝图作为委托绑定监听
	FAttibuteInfoSignature AttributeInfoDelegate;

protected:

	UPROPERTY(EditDefaultsOnly)//EditDefaultsOnly只能在UE面板编辑
	TObjectPtr<UAttributeInfo> AttributeInfo;
private:
	void BroadcastAttributeInfo(const FGameplayTag& AttributeTag, const FGameplayAttribute& Attribute) const;
};
