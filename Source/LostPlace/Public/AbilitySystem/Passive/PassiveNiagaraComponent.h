// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "NiagaraComponent.h"
#include "PassiveNiagaraComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class LOSTPLACE_API UPassiveNiagaraComponent : public UNiagaraComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UPassiveNiagaraComponent();

	//激活此被动技能特效的技能标签
	UPROPERTY(EditDefaultsOnly)
	FGameplayTag PassiveSpellTag;

protected:
	virtual void BeginPlay() override;

	/**
	 * 监听技能变动后的委托回调，用于设置此实例是否需要激活
	 * @param AbilityTag 对应的技能的标签
	 * @param bActivate 激活还是关闭
	 */
	void OnPassiveActivate(const FGameplayTag& AbilityTag, bool bActivate);
};
