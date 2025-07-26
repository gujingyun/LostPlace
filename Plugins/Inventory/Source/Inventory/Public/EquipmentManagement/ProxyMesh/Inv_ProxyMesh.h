// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Inv_ProxyMesh.generated.h"

class UInv_EquipmentComponent;
class USkeletalMeshComponent;
/**
 *
 */
UCLASS()
class INVENTORY_API AInv_ProxyMesh : public AActor
{
	GENERATED_BODY()

public:
	AInv_ProxyMesh();
	USkeletalMeshComponent* GetMesh() const { return Mesh; }
protected:
	virtual void BeginPlay() override;

private:

	// 这是玩家控制的角色上的网格。
	TWeakObjectPtr<USkeletalMeshComponent> SourceMesh;

	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TObjectPtr<UInv_EquipmentComponent> EquipmentComponent;

	// 这是我们将在 Inventory Menu 中看到的代理网格。
	UPROPERTY(VisibleAnywhere, Category="Inventory")
	TObjectPtr<USkeletalMeshComponent> Mesh;

	//延时加载
	FTimerHandle TimerForNextTick;
	void DelayedInitializeOwner();
	void DelayedInitialization();
};