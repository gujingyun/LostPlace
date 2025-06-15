#pragma once //预处理指令 确保这个头文件只被包含（include）一次，防止重复定义。

#include "GameplayEffectTypes.h"
#include "LPAbilityTypes.generated.h"
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FDamageEffectParams
{
	GENERATED_BODY()

	FDamageEffectParams(){}

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UObject> WorldContextObject = nullptr; //当前场景上下文对象

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UGameplayEffect> DamageGameplayEffectClass = nullptr; //需要应用的GE的类

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> SourceAbilitySystemComponent; //源ASC

	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<UAbilitySystemComponent> TargetAbilitySystemComponent; //目标ASC

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag DamageType; //技能造成的伤害类型
	
	UPROPERTY(BlueprintReadWrite)
	float BaseDamage = 0.f; //技能造成的伤害
	
	UPROPERTY(BlueprintReadWrite)
	float AbilityLevel = 1.f; //技能等级

	// UPROPERTY()
	// FGameplayTag DeBuffDamageType = FGameplayTag(); //负面效果伤害类型

	UPROPERTY(BlueprintReadWrite)
	float DeBuffChance = 0.f; //触发负面效果概率

	UPROPERTY(BlueprintReadWrite)
	float DeBuffDamage = 0.f; //负面效果伤害

	UPROPERTY(BlueprintReadWrite)
	float DeBuffDuration = 0.f; //负面效果持续时间

	UPROPERTY(BlueprintReadWrite)
	float DeBuffFrequency = 0.f; //负面效果触发频率

	UPROPERTY(BlueprintReadWrite)
	float DeathImpulseMagnitude = 0.f; //死亡时受到的冲击力
	
	UPROPERTY(BlueprintReadWrite)
	FVector DeathImpulse = FVector::ZeroVector; //死亡时受到的冲击力方向


	UPROPERTY(BlueprintReadWrite)
	float KnockbackForceMagnitude = 0.f;

	UPROPERTY(BlueprintReadWrite)
	float KnockbackChance = 0.f;
	
	UPROPERTY(BlueprintReadWrite)
	FVector KnockbackForce = FVector::ZeroVector; //攻击时击退的方向
	
	
};

USTRUCT(BlueprintType) //在蓝图中可作为类型使用
struct FLPGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY() //宏 自动生成构造函数、析构函数、拷贝构造函数等
public:

	bool IsBlockedHit() const { return bIsBlockedHit; }
	bool IsCriticalHit() const { return bIsCriticalHit; }

	void SetIsBlockedHit(const bool bInIsBlockedHit) { bIsBlockedHit = bInIsBlockedHit; }
	void SetIsCriticalHit(const bool bInIsCriticalHit) { bIsCriticalHit = bInIsCriticalHit; }
	
	bool IsSuccessfulDeBuff() const { return bIsSuccessfulDeBuff; } //获取 应用负面效果
	float GetDeBuffDamage() const { return DeBuffDamage; } //获取 负面效果伤害
	float GetDeBuffDuration() const { return DeBuffDuration; } //获取 负面效果持续时间
	float GetDeBuffFrequency() const { return DeBuffFrequency; } //获取 负面效果伤害触发间隔
	TSharedPtr<FGameplayTag> GetDamageType() const { return DamageType; } //获取 负面效果伤害类型
	FVector GetDeathImpulse() const { return DeathImpulse; } //获取到死亡冲击的方向和力度
	FVector GetKnockbackForce() const { return KnockbackForce; } //获取击退的方向和力度
	
	void SetIsSuccessfulDeBuff(const bool bInIsSuccessfulDeBuff) { bIsSuccessfulDeBuff = bInIsSuccessfulDeBuff; } //设置 应用负面效果
	void SetDeBuffDamage(const float InDamage) { DeBuffDamage = InDamage; } //设置 负面效果伤害
	void SetDeBuffDuration(const float InDuration) { DeBuffDuration = InDuration; } //设置 负面效果伤害
	void SetDeBuffFrequency(const float InFrequency) { DeBuffFrequency = InFrequency; } //设置 负面效果伤害
	void SetDamageType(const TSharedPtr<FGameplayTag>& InDamageType) { DamageType = InDamageType; } //设置 负面效果伤害类型
	void SetDeathImpulse(const FVector& InImpulse) { DeathImpulse = InImpulse; } //设置死亡冲击的方向和力度
	void SetKnockbackForce(const FVector& InForce) { KnockbackForce = InForce; } //设置击退的方向和力度

	/** 返回用于序列化的实际结构体 */
	virtual UScriptStruct* GetScriptStruct() const override
	{
		return StaticStruct();
	}
	/** 创建一个副本，用于后续网络复制或者后续修改 */
	virtual FLPGameplayEffectContext* Duplicate() const override
	{
		FLPGameplayEffectContext* NewContext = new FLPGameplayEffectContext();
		*NewContext = *this; //WithCopy 设置为true，就可以通过赋值操作进行拷贝
		if (GetHitResult())
		{
			// 深拷贝 hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	/** 用于序列化类的参数 */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

protected:

	UPROPERTY()
	bool bIsBlockedHit = false; //格挡

	UPROPERTY()
	bool bIsCriticalHit = false; //暴击

	UPROPERTY()
	bool bIsSuccessfulDeBuff = false; //成功应用负面效果
	
	UPROPERTY()
	float DeBuffDamage = 0.f; //负面效果每次造成的伤害

	UPROPERTY()
	float DeBuffDuration = 0.f; //负面效果持续时间

	UPROPERTY()
	float DeBuffFrequency = 0.f; //负面效果触发频率间隔

	TSharedPtr<FGameplayTag> DamageType; //负面效果的伤害类型


	UPROPERTY()
	float DeathImpulseMagnitude = 0.f; //死亡时受到的冲击力

	UPROPERTY()
	FVector DeathImpulse = FVector::ZeroVector; //死亡时受到冲击的朝向

	UPROPERTY()
	FVector KnockbackForce = FVector::ZeroVector; //攻击时击退的方向
};
template<>
struct TStructOpsTypeTraits< FLPGameplayEffectContext > : public TStructOpsTypeTraitsBase2< FGameplayEffectContext >
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true		// Necessary so that TSharedPtr<FHitResult> Data is copied around
	};
};