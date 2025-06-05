// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "NiagaraSystem.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "Interface/CombatInterface.h"
#include "CharacterBase.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;
class UGameplayEffect;
class UGameplayAbility;
class UAnimMontage;

UCLASS(Abstract)
class LOSTPLACE_API ACharacterBase : public ACharacter, public IAbilitySystemInterface, public ICombatInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ACharacterBase();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; //覆盖虚函数获取asc
	UAttributeSet* GetAttributeSet() const { return AttributeSet; } //获取as
	/* ICombatInterface战斗接口 */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override; //覆盖虚函数获取蒙太奇
	virtual void Die() override;
	virtual  FVector GetCombatSocketLocation_Implementation(const FGameplayTag CombatTag) override;
	
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;

	/* ICombatInterface战斗接口 结束 */
	
	UPROPERTY(BlueprintReadOnly) //蓝图可读
	bool bHighlighted = false; //是否高亮
	

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath();

	UPROPERTY(EditAnywhere, Category="战斗")
	TArray<FTaggedMontage> AttackMontage;


protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere,BlueprintReadOnly, Category = "战斗")
	TObjectPtr<USkeletalMeshComponent> Weapon;

	UPROPERTY(EditAnywhere, Category = "战斗")
	FName WeaponTipSocketName;

	UPROPERTY(EditAnywhere, Category = "战斗")
	FName LeftHandSocketName;

	UPROPERTY(EditAnywhere, Category = "战斗")
	FName RightHandSocketName;

	UPROPERTY(EditAnywhere, Category = "战斗")
	FName TailSocketName;
	
	bool bDead = false; //是否死亡

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;

	virtual  void InitAbilityActorInfo();

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "主要属性")
	TSubclassOf<UGameplayEffect> DefaultPrimaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "次要属性")
	TSubclassOf<UGameplayEffect> DefaultSecondaryAttributes;

	UPROPERTY(BlueprintReadOnly,EditAnywhere, Category = "重要属性")
	TSubclassOf<UGameplayEffect> DefaultVitalAttributes;
	virtual  void InitDefaultAttributes() const;
	
	void ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass,float Level) const;


	void AddCharacterAbilities();

	/* 溶解特效 */
	
	void Dissolve(); //溶解效果
	
	UFUNCTION(BlueprintImplementableEvent)
	void StartDissolveTimeline(const TArray<UMaterialInstanceDynamic*>& DynamicMaterialInstance);

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> DissolveMaterialInstance;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UMaterialInstance> WeaponDissolveMaterialInstance;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "战斗")
	UNiagaraSystem* BloodEffect; //受伤特效


	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="战斗")
	USoundBase* DeathSound; //死亡音效

	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere, Category = "能力")
	TArray<TSubclassOf<UGameplayAbility> > StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "战斗")
	TObjectPtr<UAnimMontage> HitReactMontage;
};
