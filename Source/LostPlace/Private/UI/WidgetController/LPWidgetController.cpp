// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/WidgetController/LPWidgetController.h"
void ULPWidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}
void ULPWidgetController::BroadcastInitialValues()
{
}

void ULPWidgetController::BindCallbacksToDependencies()
{
}