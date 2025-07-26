// Fill out your copyright notice in the Description page of Project Settings.

#include "Widgets/HUD/Inv_InfoMessage.h"
#include "Engine/World.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"  // 添加FTimerManager的头文件

void UInv_InfoMessage::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	Text_Message->SetText(FText::FromString(""));
	MessageHide();
}

void UInv_InfoMessage::SetMessage(const FText& Message)
{
	Text_Message->SetText(Message);

	if (!bIsMessageActive)
	{
		MessageShow();
	}
	bIsMessageActive = true;

	GetWorld()->GetTimerManager().SetTimer(MessageTimer,[this]()
		{
			MessageHide();
			bIsMessageActive = false;
		},MessageLifetime,false
	);
}
