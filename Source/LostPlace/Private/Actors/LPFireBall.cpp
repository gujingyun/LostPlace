// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LPFireBall.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "LPGameplayTags.h"
#include "GameplayCueManager.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "Components/AudioComponent.h"


// Sets default values
ALPFireBall::ALPFireBall()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void ALPFireBall::BeginPlay()
{
	Super::BeginPlay();
	StartOutgoingTimeline();
}

void ALPFireBall::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!IsValidOverlap(OtherActor)) return;

	if (HasAuthority())
	{
		if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
		{
			const FVector DeathImpulse = GetActorForwardVector() * DamageEffectParams.DeathImpulseMagnitude;
			DamageEffectParams.DeathImpulse = DeathImpulse;
			
			DamageEffectParams.TargetAbilitySystemComponent = TargetASC;
			ULPAbilitySystemLibrary::ApplyDamageEffect(DamageEffectParams);
		}
	}
}

void ALPFireBall::OnHit()
{
	if(GetOwner())
	{
		//设置GameplayCue播放位置
		FGameplayCueParameters Parameters;
		Parameters.Location = GetActorLocation();
		UGameplayCueManager::ExecuteGameplayCue_NonReplicated(GetOwner(), FLPGameplayTags::Get().GameplayCue_FireBlast, Parameters);
	}
	if (LoopingSoundComponent)
	{
		LoopingSoundComponent->Stop();
		LoopingSoundComponent->DestroyComponent();
	}
	bHit = true;
}


