// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/EffectActorBase.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
AEffectActorBase::AEffectActorBase()
{
	PrimaryActorTick.bCanEverTick = false;
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(TEXT("RootComponent")));
}

void AEffectActorBase::BeginPlay()
{
	Super::BeginPlay();
	//设置初始位置
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
	CalculatedRotation = GetActorRotation();
}

void AEffectActorBase::OnOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
	}
	if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
	}
}

void AEffectActorBase::OnEndOverlap(AActor* TargetActor)
{
	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	//添加效果
	if(InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}

	if(DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}

	if(InfinityEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfinityGameplayEffectClass);
	}

	//删除效果
	if(InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if(!IsValid(TargetASC)) return;

		//创建存储需要移除的效果句柄存储Key，用于遍历完成后移除效果
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;

		//循环map内存的数据
		for(TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*> HandlePair : ActiveEffectHandles)
		{
			//判断是否ASC相同
			if(TargetASC == HandlePair.Value)
			{
				//通过句柄将效果移除，注意，有可能有多层效果，不能将其它层的效果也移除掉，所以只移除一层
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key, 1);
				//添加到移除列表
				HandlesToRemove.Add(HandlePair.Key);
			}
		}

		//遍历完成后，在Map中将移除效果的KeyValue删除
		for(auto& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
}

void AEffectActorBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//更新当前Actor的存在时间
	RunningTime += DeltaTime;
	ItemMovement(DeltaTime);
}

void AEffectActorBase::ItemMovement(float DeltaSeconds)
{
	//更新转向
	if(bRotates)
	{
		const FRotator DeltaRotation(0.f, DeltaSeconds * RotationRate, 0.f);
		CalculatedRotation = UKismetMathLibrary::ComposeRotators(CalculatedRotation, DeltaRotation);
	}
	//更新位置
	if(bSinusoidalMovement)
	{
		const float Sine = SineAmplitude * FMath::Sin(RunningTime * SinePeriod * 6.28318f);
		CalculatedLocation = InitialLocation + FVector(0.f, 0.f, Sine);
	}
}


void AEffectActorBase::StartSinusoidalMovement()
{
	bSinusoidalMovement = true;
	InitialLocation = GetActorLocation();
	CalculatedLocation = InitialLocation;
}

void AEffectActorBase::StartRotation()
{
	bRotates = true;
	CalculatedRotation = GetActorRotation();
}

void AEffectActorBase::ApplyEffectToTarget(AActor* TargetActor, TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	/**
	* 默认自己编写从actor身上获取ASC的方式
	* IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetActor); //判断当前actor是否有技能系统接口
	if(ASCInterface)
	{
		UAbilitySystemComponent* TargetASC = ASCInterface->GetAbilitySystemComponent(); 
	}
	 */

	if (TargetActor->ActorHasTag(FName("Enemy")) && !bApplyEffectsToEnemies) return;
	
	//获取ASC
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if(TargetASC == nullptr) return;
	
	check(GameplayEffectClass);
	//创建Effect的句柄 包含了实例化Effect所需数据
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	//设置创建Effect的对象
	EffectContextHandle.AddSourceObject(this);
	//Effect的实例化后的句柄，可以通过此来寻找调用
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass, ActorLevel, EffectContextHandle);
	//从句柄中获取到实例的地址，并被应用。
	const FActiveGameplayEffectHandle ActiveGameplayEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get());

	//从句柄中获取到定义的对象，并判断设置的
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	//在是无限时间效果和需要在结束时清除掉时，将效果句柄添加到map
	if(bIsInfinite && InfinityEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveGameplayEffectHandle, TargetASC);
	}

	if (bDestroyOnEffectApplication && !bIsInfinite)
	{
		Destroy();
	}	
}
