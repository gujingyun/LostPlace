// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "LPDamageGameplayAbility.h"
#include "Actors/Projectile.h"
#include "LPProjectileSpell.generated.h"
class AProjectile;
class UGameplayEffect;
class UAbilitySystemComponent;

struct FGameplayTag;
/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPProjectileSpell : public ULPDamageGameplayAbility
{
	GENERATED_BODY()

public:
	
	UPROPERTY(EditAnywhere,BlueprintReadOnly)
	TSubclassOf<AProjectile> ProjectileClass;

	UFUNCTION(BlueprintCallable)
	void SpawnProjectile(const FVector& ProjectileTargetLocation,const FGameplayTag& SocketTag, const bool bOverridePitch = false, const float PitchOverride = 0.f);

	UPROPERTY(EditAnywhere)
	int32 NumProjectiles = 5;
protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;


};
