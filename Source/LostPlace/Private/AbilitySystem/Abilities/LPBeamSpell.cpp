// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPBeamSpell.h"

#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetSystemLibrary.h"

void ULPBeamSpell::StoreMouseDataInfo(const FHitResult& HitResult)
{
	//判断当前是否拾取到内容
	if(HitResult.bBlockingHit)
	{
		MouseHitLocation = HitResult.ImpactPoint;
		MouseHitActor = HitResult.GetActor();
	}
	else
	{
		//取消技能
		CancelAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true);
	}
}

void ULPBeamSpell::StoreOwnerVariables()
{
	if(CurrentActorInfo)
	{
		OwnerPlayerController = CurrentActorInfo->PlayerController.Get();
		OwnerCharacter = Cast<ACharacter>(CurrentActorInfo->AvatarActor);
	}
}

void ULPBeamSpell::TraceFirstTarget(const FVector& BeamTargetLocation)
{
	//确保所有者继承了战斗接口
	if(OwnerCharacter && OwnerCharacter->Implements<UCombatInterface>())
	{
		//获取到武器
		if(USkeletalMeshComponent* Weapon = ICombatInterface::Execute_GetWeapon(OwnerCharacter))
		{
			TArray<AActor*> ActorsToIgnore; //当前需要忽略的对象数组
			ActorsToIgnore.Add(OwnerCharacter);// 将自身忽略掉
			
			const FVector SocketLocation = Weapon->GetSocketLocation(FName("TipSocket")); //获取技能发射位置

			FHitResult HitResult; //命中结果的纯粹对象

			//通过武器发射位置和命中位置生成一条球形线，获取第一个命中的结果
			UKismetSystemLibrary::SphereTraceSingle(
				OwnerCharacter,
				SocketLocation,
				BeamTargetLocation,
				10.f,
				TraceTypeQuery1,
				false,
				ActorsToIgnore,
				EDrawDebugTrace::None, //如果需要debug，将其设置ForDuration，如果关闭设置为None
				HitResult,
				true);

			//如果有命中的结果，修改拾取结果
			if(HitResult.bBlockingHit)
			{
				MouseHitLocation = HitResult.ImpactPoint;
				MouseHitActor = HitResult.GetActor();
			}
		}
	}
	if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(OwnerCharacter))
	{
		if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &ULPBeamSpell::PrimaryTargetDied))
		{
			CombatInterface->GetOnDeathDelegate().AddDynamic(this, &ULPBeamSpell::PrimaryTargetDied);
		}
		
	}
}

void ULPBeamSpell::StoreAdditionalTargets(TArray<AActor*>& OutAdditionalTargets)
{
	TArray<AActor*> ActorsToIgnore; //遍历时忽略的数组
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo()); //忽略自身
	ActorsToIgnore.Add(MouseHitActor); //忽略鼠标命中的的敌人
	
	TArray<AActor*> OverlappingActors; //存放遍历结果的数组

	//通过封装的函数获取到技能范围内散射的目标
	ULPAbilitySystemLibrary::GetLivePlayersWithinRadius(
		GetAvatarActorFromActorInfo(),
		OverlappingActors,
		ActorsToIgnore,
		850.f,
		MouseHitActor->GetActorLocation());

	int32 NumAdditionalTargets = FMath::Min(GetAbilityLevel() - 1, MaxNumShockTargets);
	// int32 NumAdditionalTargets = 5;

	//通过自定义函数来获取最近的几个目标
	ULPAbilitySystemLibrary::GetClosestTargets(NumAdditionalTargets, OverlappingActors, OutAdditionalTargets, MouseHitActor->GetActorLocation());

	for (AActor* Target : OutAdditionalTargets)
	{
		if (ICombatInterface* CombatInterface = Cast<ICombatInterface>(Target))
		{
			if (!CombatInterface->GetOnDeathDelegate().IsAlreadyBound(this, &ULPBeamSpell::AdditionalTargetDied))
			{
				CombatInterface->GetOnDeathDelegate().AddDynamic(this, &ULPBeamSpell::AdditionalTargetDied);
			}
		
		}
	}
}
