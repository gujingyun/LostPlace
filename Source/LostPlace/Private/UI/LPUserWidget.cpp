// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LPUserWidget.h"

void ULPUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
