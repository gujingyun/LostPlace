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

class UPassiveNiagaraComponent;
class UDebuffNiagaraComponent;
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
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	/**
	 * 覆写 应用伤害给自身
	 * @see https://www.unrealengine.com/blog/damage-in-ue4
	 * @param DamageAmount		要施加的伤害数值
	 * @param DamageEvent		描述伤害细节的结构体，支持不同类型的伤害，如普通伤害、点伤害（FPointDamageEvent）、范围伤害（FRadialDamageEvent）等。
	 * @param EventInstigator	负责造成伤害的 Controller，通常是玩家或 AI 的控制器。
	 * @param DamageCauser		直接造成伤害的 Actor，例如爆炸物、子弹或掉落的石头。
	 * @return					返回实际应用的伤害值。这允许目标修改或减少伤害，然后将最终的值返回。
	 */
	virtual float TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; //覆盖虚函数获取asc
	UAttributeSet* GetAttributeSet() const { return AttributeSet; } //获取as
	/* ICombatInterface战斗接口 */
	virtual UAnimMontage* GetHitReactMontage_Implementation() override; //覆盖虚函数获取蒙太奇
	virtual void Die(const FVector& DeathImpulse) override;
	virtual  FVector GetCombatSocketLocation_Implementation(const FGameplayTag CombatTag) override;
	
	virtual bool IsDead_Implementation() const override;
	virtual AActor* GetAvatar_Implementation() override;
	virtual TArray<FTaggedMontage> GetAttackMontages_Implementation() override;
	virtual UNiagaraSystem* GetBloodEffect_Implementation() override;
	virtual FTaggedMontage GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag) override;

	virtual int32 GetMinionCount_Implementation() override; //获取等级

	virtual void IncrementMinionCount_Implementation(const int32 Amount) override;

	virtual ECharacterClass GetCharacterClass_Implementation() override;

	virtual FOnASCRegistered& GetOnASCRegisteredDelegate() override;
	
	virtual FOnDeath& GetOnDeathDelegate() override;

	virtual USkeletalMeshComponent* GetWeapon_Implementation() override;

	virtual void SetIsBeingShocked_Implementation(bool bInShock) override;
	virtual bool IsBeingShocked_Implementation() const override;

	virtual FOnDamageSignature& GetOnDamageDelegate() override;

	/* ICombatInterface战斗接口 结束 */

	FOnASCRegistered OnASCRegistered; //ASC注册成功委托
	FOnDeath OnDeath; //角色死亡后触发的死亡委托
	FOnDamageSignature OnDamageDelegate; //传入伤害后得到结果后的委托
	
	UPROPERTY(BlueprintReadOnly) //蓝图可读
	bool bHighlighted = false; //是否高亮
	

	UFUNCTION(NetMulticast, Reliable)
	virtual void MulticastHandleDeath(const FVector& DeathImpulse);

	
	UPROPERTY(EditAnywhere, Category="战斗")
	TArray<FTaggedMontage> AttackMontage;
	
	//当前角色是否处于眩晕状态
	UPROPERTY(ReplicatedUsing=OnRep_Burned, BlueprintReadOnly)
	bool bIsBurned = false;
	
	//当前角色是否处于眩晕状态
	UPROPERTY(ReplicatedUsing=OnRep_Stunned, BlueprintReadOnly)
	bool bIsStunned = false;

	UPROPERTY(Replicated, BlueprintReadOnly)
	bool IsBeingShocked = false;
	
	//注册用于监听负面标签变动的函数
	void DeBuffRegisterChanged();


	//当前角色的最大移动速度
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Combat")
	float BaseWalkSpeed = 600.f;

	UFUNCTION()
	virtual void OnRep_Stunned();
	
	UFUNCTION()
	virtual void OnRep_Burned();

	void SetCharacterClass(const ECharacterClass InClass) { CharacterClass = InClass; };

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

	UPROPERTY(BlueprintReadOnly)
	bool bDead = false; //是否死亡

	//眩晕标签变动后的回调
	virtual void StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount);



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

	UPROPERTY(BlueprintReadOnly, Category="战斗")
	int32 MinionsCount = 0; //仆从数量

	UPROPERTY(EditAnywhere,BlueprintReadOnly,Category="角色默认类")
	ECharacterClass CharacterClass = ECharacterClass::Warrior;

	UPROPERTY(VisibleAnywhere, Category="战斗")
	TObjectPtr<UDebuffNiagaraComponent> BurnDebuffComponent;

	UPROPERTY(VisibleAnywhere, Category="战斗")
	TObjectPtr<UDebuffNiagaraComponent> StunDebuffComponent;
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	UPROPERTY(EditAnywhere, Category = "能力")
	TArray<TSubclassOf<UGameplayAbility> > StartupAbilities;

	UPROPERTY(EditAnywhere, Category = "能力")
	TArray<TSubclassOf<UGameplayAbility> > StartupPassiveAbilities;
	
	UPROPERTY(EditAnywhere, Category = "战斗")
	TObjectPtr<UAnimMontage> HitReactMontage;

	//光环被动技能特效组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> HaloOfProtectionNiagaraComponent;

	//回血被动技能特效组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> LifeSiphonNiagaraComponent;

	//回蓝被动技能特效组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPassiveNiagaraComponent> ManaSiphonNiagaraComponent;

	//被动技能挂载的组件
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USceneComponent> EffectAttachComponent;

};
