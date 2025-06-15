// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPProjectileSpell.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Interface/CombatInterface.h"
#include "LPGameplayTags.h"

void ULPProjectileSpell::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                         const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                         const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

}
void ULPProjectileSpell::SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag, const bool bOverridePitch, const float PitchOverride)
{
	const bool bIsServer = GetAvatarActorFromActorInfo()->HasAuthority(); //判断此函数是否在服务器运行
	if (!bIsServer) return;

	FTransform SpawnTransform;
	const FVector SocketLocation = ICombatInterface::Execute_GetCombatSocketLocation(GetAvatarActorFromActorInfo(),SocketTag);
	FRotator Rotation = (ProjectileTargetLocation - SocketLocation).Rotation(); //将方向转为旋转
	if(bOverridePitch)
	{
		Rotation.Pitch = PitchOverride; //覆写发射角度
	}

	
	SpawnTransform.SetLocation(SocketLocation);
	SpawnTransform.SetRotation(Rotation.Quaternion());
	
	//SpawnActorDeferred将异步创建实例，在实例创建完成时，相应的数据已经应用到了实例身上
	AProjectile* Projectile = GetWorld()->SpawnActorDeferred<AProjectile>(
		ProjectileClass,
		SpawnTransform,
		GetOwningActorFromActorInfo(),
		Cast<APawn>(GetOwningActorFromActorInfo()),
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
	
	FDamageEffectParams DamageEffectParams = MakeDamageEffectParamsFromClassDefaults();
	Projectile->DamageEffectParams = DamageEffectParams;
	//确保变换设置被正确应用
	Projectile->FinishSpawning(SpawnTransform);
}


