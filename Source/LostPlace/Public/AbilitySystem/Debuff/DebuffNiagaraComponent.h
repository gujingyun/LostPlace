// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "DebuffNiagaraComponent.generated.h"

/**
 * 
 */
UCLASS()
class LOSTPLACE_API UDebuffNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()
public:
	UDebuffNiagaraComponent();
	
	UPROPERTY(VisibleAnywhere)
	FGameplayTag DeBuffTag; //用来标识粒子系统的标签

protected:
	virtual void BeginPlay() override; //覆写开始运行
	
	void DeBuffTagChanged(const FGameplayTag CallbackTag, int32 NewCount); //当前的负面标签变动回调

	UFUNCTION()
	void OnOwnerDeath(AActor* DeadActor); //在角色死亡时的回调
};
