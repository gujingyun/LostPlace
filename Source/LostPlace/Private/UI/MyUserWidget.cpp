// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyUserWidget.h"

void UMyUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
