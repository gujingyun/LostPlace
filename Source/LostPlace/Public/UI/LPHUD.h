// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPUserWidget.h"
#include "GameFramework/HUD.h"
#include "WidgetController/OverlayWidgetController.h"
#include "WidgetController/AttributeMenuWidgetController.h"
#include "LPHUD.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ALPHUD : public AHUD
{
	GENERATED_BODY()
public:
	
	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

	
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);

private:
	
	UPROPERTY()
	TObjectPtr<ULPUserWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<ULPUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;


	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;
};
