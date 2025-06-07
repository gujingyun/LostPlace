// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/LPHUD.h"


UOverlayWidgetController* ALPHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if(OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}
void ALPHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass 未设置，请在HUD上面设置")); //会打印相关信息到log列表
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass 未设置，请在HUD上面设置"));
	
	ULPUserWidget* Widget = CreateWidget<ULPUserWidget>(GetWorld(), OverlayWidgetClass); //创建Overlay用户控件
	OverlayWidget = Cast<ULPUserWidget>(Widget);

	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS); //创建参数结构体
	OverlayWidgetController = GetOverlayWidgetController(WidgetControllerParams); //获取控制器层

	OverlayWidget->SetWidgetController(OverlayWidgetController); //设置用户控件的控制器层
	OverlayWidgetController->BroadcastInitialValues(); //初始化广播的值
	// OverlayWidgetController->BindCallbacksToDependencies(); //绑定监听数值变化
	Widget->AddToViewport(); //添加到视口

}

UAttributeMenuWidgetController* ALPHUD::GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if(AttributeMenuWidgetController == nullptr)
	{
		AttributeMenuWidgetController = NewObject<UAttributeMenuWidgetController>(this, AttributeMenuWidgetControllerClass);
		AttributeMenuWidgetController->SetWidgetControllerParams(WCParams);
		AttributeMenuWidgetController->BindCallbacksToDependencies();
	}
	return AttributeMenuWidgetController;
}
