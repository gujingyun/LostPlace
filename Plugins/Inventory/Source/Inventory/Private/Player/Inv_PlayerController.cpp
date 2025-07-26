// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/Inv_PlayerController.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h" 
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Interaction/Inv_Highlightable.h"
#include "InventoryManagement/Components/Inv_InventoryComponent.h"
#include "InventoryManagement/Utils/Inv_InventoryStatics.h"
#include "Items/Components/Inv_ItemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Widgets/HUD/Inv_HUDWidget.h"

AInv_PlayerController::AInv_PlayerController()
{
	PrimaryActorTick.bCanEverTick = true;
	TraceLength = 500.0f;
	ItemTraceChannel = ECC_GameTraceChannel1;
}

void AInv_PlayerController::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	TraceForItem();
}

void AInv_PlayerController::ToggleInventory()
{
	if (!InventoryComponent.IsValid()) return;
	InventoryComponent->ToggleInventoryMenu();
	if (InventoryComponent->IsMenuOpen())
	{
		HUDWidget->SetVisibility(ESlateVisibility::Hidden);
	}
	else
	{
		HUDWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void AInv_PlayerController::BeginPlay()
{
	Super::BeginPlay();
	
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (IsValid(Subsystem))
	{
		Subsystem->AddMappingContext(DefaultIMC, 0);
	}

	InventoryComponent = FindComponentByClass<UInv_InventoryComponent>();
	CreateHUDWidget();
}

void AInv_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent);
	
	EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::PrimaryInteract);
	EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ThisClass::ToggleInventory);
}

void AInv_PlayerController::PrimaryInteract()
{
	if (!ThisItem.IsValid()) return;

	UInv_ItemComponent* ItemComponent = ThisItem->FindComponentByClass<UInv_ItemComponent>();
	if (!IsValid(ItemComponent) || !InventoryComponent.IsValid()) return;
	InventoryComponent->TryAddItem(ItemComponent);
	UpdateHUDForActor(ThisItem.Get());
}

void AInv_PlayerController::CreateHUDWidget()
{
	if (!IsLocalController()) return;

	HUDWidget = CreateWidget<UInv_HUDWidget>(this, HUDWidgetClass);

	if (IsValid(HUDWidget))
	{
		HUDWidget->AddToViewport();
	}
}

// 实现
void AInv_PlayerController::SetActorHighlight(AActor* Actor, bool bHighlight)
{
	if (!IsValid(Actor)) return;
    
	if (UActorComponent* Highlightable = Actor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
	{
		if (bHighlight)
		{
			IInv_Highlightable::Execute_Highlight(Highlightable);
		}
		else
		{
			IInv_Highlightable::Execute_UnHighlight(Highlightable);
		}
	}
}
 
void AInv_PlayerController::UpdateHUDForActor(AActor* Actor)
{
	if (!IsValid(HUDWidget)) return;
    
	if (IsValid(Actor))
	{
		UInv_ItemComponent* ItemComponent = Actor->FindComponentByClass<UInv_ItemComponent>();
		if (IsValid(ItemComponent))
		{
			HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
		}
	}
	else
	{
		HUDWidget->HidePickupMessage();
	}
}
 
// 重构后的TraceForItem()
void AInv_PlayerController::TraceForItem()
{
	FVector PlayerLocation = FVector::ZeroVector;
	if (GetPawn())
	{
		PlayerLocation = GetPawn()->GetActorLocation();
	}
    
	LastItem = ThisItem;
	TArray<AActor*> OutOverlappingActors;
	TArray<AActor*> ActorsToIgnore;
	UInv_InventoryStatics::GetItemsWithinRadius(this, OutOverlappingActors, ActorsToIgnore, 200.0f, PlayerLocation);
	ThisItem = UInv_InventoryStatics::GetNearestActor(OutOverlappingActors, PlayerLocation);
    
	// 如果目标没有变化，直接返回
	if (LastItem == ThisItem) return;
    
	// 取消之前Actor的高亮
	SetActorHighlight(LastItem.Get(), false);
    
	// 设置新Actor的高亮和UI
	SetActorHighlight(ThisItem.Get(), true);
	UpdateHUDForActor(ThisItem.Get());
}
// void AInv_PlayerController::TraceForItem()
// {
// 	if (!IsValid(GEngine) || !IsValid(GEngine->GameViewport)) return;
// 	FVector2D ViewportSize;
// 	GEngine->GameViewport->GetViewportSize(ViewportSize);
// 	const FVector2D ViewportCenter = ViewportSize / 2.0f;
// 	FVector TraceStart;
// 	FVector Forward;
// 	if (!UGameplayStatics::DeprojectScreenToWorld(this, ViewportCenter, TraceStart, Forward)) return;
//
// 	const FVector TraceEnd = TraceStart + (Forward * TraceLength);
//
// 	FHitResult HitResult;
// 	GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ItemTraceChannel);
//
//
// 	LastActor = ThisActor;
// 	ThisActor = HitResult.GetActor();
// 	if (!ThisActor.IsValid())
// 	{
// 		if (IsValid(HUDWidget)) HUDWidget->HidePickupMessage();
// 	}
// 	if (LastActor == ThisActor) return;
//
// 	if (ThisActor.IsValid())
// 	{
// 		if (UActorComponent* Highlightable = ThisActor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
// 		{
// 			IInv_Highlightable::Execute_Highlight(Highlightable);
// 		}
// 		
// 		UInv_ItemComponent* ItemComponent = ThisActor->FindComponentByClass<UInv_ItemComponent>();
// 		if (!IsValid(ItemComponent)) return;
//
// 		if (IsValid(HUDWidget)) HUDWidget->ShowPickupMessage(ItemComponent->GetPickupMessage());
// 		
// 	}
//
// 	if (LastActor.IsValid())
// 	{
// 		if (UActorComponent* Highlightable = LastActor->FindComponentByInterface(UInv_Highlightable::StaticClass()); IsValid(Highlightable))
// 		{
// 			IInv_Highlightable::Execute_UnHighlight(Highlightable);
// 		}
// 	}
// }
