// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterBase.h"
#include "GameFramework/Character.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "Components/WidgetComponent.h"
#include "Interface/EnemyInterface.h"
#include "Kismet/GameplayStatics.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "LPEnemyCharacter.generated.h"

class UWidgetComponent;

class ALPAIController;

UCLASS()
class LOSTPLACE_API ALPEnemyCharacter : public ACharacterBase, public IEnemyInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ALPEnemyCharacter();

	virtual void PossessedBy(AController* NewController) override;
	/** 实现IEnemyInterface接口 */
	virtual void HighlightActor() override; //高亮
	virtual void UnHighlightActor() override; //取消高亮
	virtual void SetCombatTarget_Implementation(AActor* InCombatTarget) override;
	virtual AActor* GetCombatTarget_Implementation() const override;
	/** 实现IEnemyInterface接口结束 */
	
	/** 实现ICombatInterface接口 */
	virtual int32 GetPlayerLevel() override; //获取等级
	/** 实现ICombatInterface接口结束 */

	virtual void Die() override;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnAttributeChangedSignature OnMaxHealthChanged;

	void HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount);
	
	UPROPERTY(BlueprintReadOnly, Category="战斗")
	bool bHitReacting = false; //当前是否处于被攻击状态

	UPROPERTY(EditAnywhere, Category="战斗")
	float BaseWalkSpeed = 250.f; //当前角色的最大移动速度
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="战斗")
	float LifeSpan = 5.f; //设置死亡后的存在时间

	UPROPERTY(BlueprintReadWrite, Category="战斗")
	TObjectPtr<AActor> CombatTarget; //攻击目标;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual  void InitAbilityActorInfo() override;

	virtual  void InitDefaultAttributes() const  override;
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="角色默认类")
	int32 Level = 1;

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="角色默认类")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Widget")
	TObjectPtr<UWidgetComponent> HealthBar;

	UPROPERTY(EditAnywhere, Category="AI")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY()
	TObjectPtr<ALPAIController> LPAIController;
	
	


};
