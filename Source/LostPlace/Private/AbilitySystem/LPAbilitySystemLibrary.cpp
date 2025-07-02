// Fill out your copyright notice in the Description page of Project Settings.
#include "AbilitySystem/LPAbilitySystemLibrary.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "LPAbilityTypes.h"
#include "LPGameplayTags.h"
#include "Core/LPGameMode.h"
#include "Engine/OverlapResult.h"
#include "Interface/CombatInterface.h"
#include "Kismet/GameplayStatics.h"
#include "Player/LPPlayerState.h"
#include "UI/LPHUD.h"


bool ULPAbilitySystemLibrary::MakeOverlayWidgetControllerPrParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams,ALPHUD*& OutHUD)
{
	//获取到playerController， 需要传入一个世界空间上下文的对象，用于得到对应世界中的PC列表，0为本地使用的PC
	if(APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		OutHUD = Cast<ALPHUD>(PC->GetHUD());
		//从PC获取到HUD，我们就可以从HUD获得对应的Controller
		if(OutHUD)
		{
			ALPPlayerState* PS = PC->GetPlayerState<ALPPlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			OutWCParams.AttributeSet = AS;
			OutWCParams.AbilitySystemComponent = ASC;
			OutWCParams.PlayerState = PS;
			OutWCParams.PlayerController = PC;
			return  true;
		}
	}
	return false;
}

UOverlayWidgetController* ULPAbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ALPHUD* HUD = nullptr;
	if(MakeOverlayWidgetControllerPrParams(WorldContextObject, WCParams,HUD))
	{
		return HUD->GetOverlayWidgetController(WCParams);
	}
	return nullptr;
}
UAttributeMenuWidgetController* ULPAbilitySystemLibrary::GetAttributeMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ALPHUD* HUD = nullptr;
	if(MakeOverlayWidgetControllerPrParams(WorldContextObject, WCParams,HUD))
	{
		return HUD->GetAttributeMenuWidgetController(WCParams);
	}
	return nullptr;
}

USpellMenuWidgetController* ULPAbilitySystemLibrary::GetSpellMenuWidgetController(const UObject* WorldContextObject)
{
	FWidgetControllerParams WCParams;
	ALPHUD* HUD = nullptr;
	if(MakeOverlayWidgetControllerPrParams(WorldContextObject, WCParams,HUD))
	{
		return HUD->GetSpellMenuWidgetController(WCParams);
	}
	return nullptr;
}

void ULPAbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
                                                          ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	FCharacterClassDefaultInfo ClassDefaultInfo = ClassInfo->GetClassDefaultInfo(CharacterClass);
	FGameplayEffectContextHandle PrimaryContextHandle = ASC->MakeEffectContext();
	PrimaryContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle =  ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, ASC->MakeEffectContext());
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	//设置次级属性
	FGameplayEffectContextHandle SecondaryContextHandle = ASC->MakeEffectContext();
	SecondaryContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle SecondarySpecHandle = ASC->MakeOutgoingSpec(ClassInfo->SecondaryAttributes, Level, SecondaryContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondarySpecHandle.Data.Get());

	//填充血量和蓝量
	FGameplayEffectContextHandle VitalContextHandle = ASC->MakeEffectContext();
	VitalContextHandle.AddSourceObject(WorldContextObject);
	const FGameplayEffectSpecHandle VitalSpecHandle = ASC->MakeOutgoingSpec(ClassInfo->VitalAttributes, Level, VitalContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalSpecHandle.Data.Get());
}
void ULPAbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	//从实例获取到关卡角色的配置
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);

	if (CharacterClassInfo == nullptr) return;
	//遍历角色拥有的技能数组
	for(const TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1); //创建技能实例
		ASC->GiveAbility(AbilitySpec); //只应用不激活
	}
	const FCharacterClassDefaultInfo& DefaultInfo  = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (ASC->GetAvatarActor()->Implements<UCombatInterface>())
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, ICombatInterface::Execute_GetPlayerLevel(ASC->GetAvatarActor())); //创建技能实例
			ASC->GiveAbility(AbilitySpec); //只应用不激活
		}
	}
	
}

UCharacterClassInfo* ULPAbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const ALPGameMode* GameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr)  return nullptr;
	//从实例获取到关卡角色的配置
	return GameMode->CharacterClassInfo;
}

UAbilityInfo* ULPAbilitySystemLibrary::GetAbilityInfo(const UObject* WorldContextObject)
{
	//获取到当前关卡的GameMode实例
	const ALPGameMode* GameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(WorldContextObject));
	if(GameMode == nullptr)  return nullptr;

	return GameMode->AbilityInfo;
}

bool ULPAbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->IsBlockedHit();
	}
	return false;
}

bool ULPAbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->IsCriticalHit();
	}
	return false;
}

void ULPAbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlockedHit)
{
	if (FLPGameplayEffectContext* LPEffectContext = static_cast<FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		LPEffectContext->SetIsBlockedHit(bIsBlockedHit);
	}
}

void ULPAbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle, bool bIsCriticalHit)
{
	if (FLPGameplayEffectContext* LPEffectContext = static_cast<FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		LPEffectContext->SetIsCriticalHit(bIsCriticalHit);
	}
}

bool ULPAbilitySystemLibrary::IsSuccessfulDeBuff(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->IsSuccessfulDeBuff();
	}
	return false;
}

float ULPAbilitySystemLibrary::GetDeBuffDamage(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->GetDeBuffDamage();
	}
	return 0.f;
}

float ULPAbilitySystemLibrary::GetDeBuffDuration(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->GetDeBuffDuration();
	}
	return 0.f;
}

float ULPAbilitySystemLibrary::GetDeBuffFrequency(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{

		return LPEffectContext->GetDeBuffFrequency();
	}
	return 0.f;
}

FGameplayTag ULPAbilitySystemLibrary::GetDamageType(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		if (LPEffectContext->GetDamageType().IsValid())
		{
			return *LPEffectContext->GetDamageType();
		}
	}
	return FGameplayTag();
}

void ULPAbilitySystemLibrary::SetIsSuccessfulDeBuff(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsSuccessfulDeBuff)
{
	if (FLPGameplayEffectContext* LPEffectContext = static_cast<FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		LPEffectContext->SetIsSuccessfulDeBuff(bInIsSuccessfulDeBuff);
	}
}

void ULPAbilitySystemLibrary::SetDeBuff(FGameplayEffectContextHandle& EffectContextHandle, FGameplayTag& InDamageType,
	float InDamage, float InDuration, float InFrequency)
{
	FLPGameplayEffectContext* LPEffectContext = static_cast<FLPGameplayEffectContext*>(EffectContextHandle.Get());
	//通过标签创建一个共享指针
	const TSharedPtr<FGameplayTag> DamageType = MakeShared<FGameplayTag>(InDamageType);
	LPEffectContext->SetDamageType(DamageType);
	LPEffectContext->SetDeBuffDamage(InDamage);
	LPEffectContext->SetDeBuffDuration(InDuration);
	LPEffectContext->SetDeBuffFrequency(InFrequency);
}

FVector ULPAbilitySystemLibrary::GetDeathImpulse(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->GetDeathImpulse();
	}
	return FVector::ZeroVector;
}

void ULPAbilitySystemLibrary::SetDeathImpulse(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& bInDeathImpulse)
{
	if (FLPGameplayEffectContext* LPEffectContext = static_cast<FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		LPEffectContext->SetDeathImpulse(bInDeathImpulse);
	}
}

FVector ULPAbilitySystemLibrary::GetKnockbackForce(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FLPGameplayEffectContext* LPEffectContext = static_cast<const FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return LPEffectContext->GetKnockbackForce();
	}
	return FVector::ZeroVector;
}

void ULPAbilitySystemLibrary::SetKnockbackForce(FGameplayEffectContextHandle& EffectContextHandle,
	const FVector& InForce)
{
	if (FLPGameplayEffectContext* LPEffectContext = static_cast<FLPGameplayEffectContext*>(EffectContextHandle.Get()))
	{
		LPEffectContext->SetKnockbackForce(InForce);
	}
}

void ULPAbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
                                                         TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
                                                         const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams; //创建一个碰撞查询的配置
	SphereParams.AddIgnoredActors(ActorsToIgnore); //添加忽略的Actor

	TArray<FOverlapResult> Overlaps; //创建存储检索到的与碰撞体产生碰撞的Actor
	if (UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull)) //获取当前所处的场景，如果获取失败，将打印并返回Null
	{
		//获取到所有与此球体碰撞的动态物体
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for(FOverlapResult& Overlap : Overlaps) //遍历所有获取到的动态Actor
		{
			//判断当前Actor是否包含战斗接口   Overlap.GetActor() 从碰撞检测结果中获取到碰撞的Actor
			const bool ImplementsCombatInterface =  Overlap.GetActor()->Implements<UCombatInterface>();
			//判断当前Actor是否存活，如果不包含战斗接口，将不会判断存活（放置的火堆也属于动态Actor，这样保证不会报错）
			if(ImplementsCombatInterface && !ICombatInterface::Execute_IsDead(Overlap.GetActor())) 
			{
				OutOverlappingActors.AddUnique(Overlap.GetActor()); //将Actor添加到返回数组，AddUnique 只有在此Actor未被添加时，才可以添加到数组
			}
		}
	}
}

bool ULPAbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	if(FirstActor->ActorHasTag("Player"))
	{
		return !SecondActor->ActorHasTag("Player");
	}

	if(FirstActor->ActorHasTag("Enemy"))
	{
		return !SecondActor->ActorHasTag("Enemy");
	}

	return false;

}

int32 ULPAbilitySystemLibrary::GetXPRewardForClassAndLevel(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, int32 CharacterLevel)
{
	//从实例获取到关卡角色的配置
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if(CharacterClassInfo == nullptr) return 0;

	//获取到默认的基础角色数据
	const FCharacterClassDefaultInfo& ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	const float XPReward = ClassDefaultInfo.XPReward.GetValueAtLevel(CharacterLevel);

	return static_cast<int32>(XPReward);
}

FGameplayEffectContextHandle ULPAbilitySystemLibrary::ApplyDamageEffect(const FDamageEffectParams& DamageEffectParams)
{
	FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
	const AActor* SourceActor = DamageEffectParams.SourceAbilitySystemComponent->GetAvatarActor();
	FGameplayEffectContextHandle EffectContextHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeEffectContext();
	EffectContextHandle.AddSourceObject(SourceActor);
	SetDeathImpulse(EffectContextHandle, DamageEffectParams.DeathImpulse);
	SetKnockbackForce(EffectContextHandle, DamageEffectParams.KnockbackForce);
	const FGameplayEffectSpecHandle SpecHandle = DamageEffectParams.SourceAbilitySystemComponent->MakeOutgoingSpec(DamageEffectParams.DamageGameplayEffectClass, DamageEffectParams.AbilityLevel, EffectContextHandle);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, DamageEffectParams.DamageType, DamageEffectParams.BaseDamage);

	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Chance, DamageEffectParams.DeBuffChance);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Damage, DamageEffectParams.DeBuffDamage);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Duration, DamageEffectParams.DeBuffDuration);
	UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(SpecHandle, GameplayTags.DeBuff_Frequency, DamageEffectParams.DeBuffFrequency);
	
	DamageEffectParams.TargetAbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	
	return EffectContextHandle;
}
TArray<FRotator> ULPAbilitySystemLibrary::EvenlySpacedRotators(const FVector& Forward, const FVector& Axis, float Spread, int32 NumRotators)
{
	TArray<FRotator> Rotators;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis); //获取到最左侧的角度
	
	if(NumRotators > 1)
	{
		const float DeltaSpread = Spread / NumRotators; //技能分的段数

		for(int32 i=0; i<NumRotators; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5f), Axis); //获取当前分段的角度
			Rotators.Add(Direction.Rotation());
		}
	}
	else
	{
		//如果只需要一个，则将朝向放入即可
		Rotators.Add(Forward.Rotation());
	}

	return Rotators;
}

TArray<FVector> ULPAbilitySystemLibrary::EvenlyRotatedVectors(const FVector& Forward, const FVector& Axis, float Spread, int32 NumVectors)
{
	TArray<FVector> Vectors;
	
	const FVector LeftOfSpread = Forward.RotateAngleAxis(-Spread / 2.f, Axis); //获取到最左侧的角度
	
	if(NumVectors > 1)
	{
		const float DeltaSpread = Spread / NumVectors; //技能分的段数

		for(int32 i=0; i<NumVectors; i++)
		{
			const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * (i + 0.5f), Axis); //获取当前分段的角度
			Vectors.Add(Direction);
		}
	}
	else
	{
		//如果只需要一个，则将朝向放入即可
		Vectors.Add(Forward);
	}

	return Vectors;
}

void ULPAbilitySystemLibrary::GetClosestTargets(int32 MaxTargets, const TArray<AActor*>& Actors,
	TArray<AActor*>& OutClosestTargets, const FVector& Origin)
{
	//如果数量过于少，直接返回原数组
	if(Actors.Num() <= MaxTargets)
	{
		OutClosestTargets = Actors;
		return;
	}

	TArray<AActor*> ActorsToCheck = Actors; //没有引用就是复制，复制一份用于遍历
	int32 NumTargetFound = 0; //当前已经遍历出最近距离的个数

	//循环遍历，直到获得足够数量的目标时停止
	while (NumTargetFound < MaxTargets)
	{
		if(ActorsToCheck.Num() == 0) break; //如果没有可遍历内容，将跳出循环
		double ClosestDistance = TNumericLimits<double>::Max(); //记录中心于目标的位置，如果有更小的将被替换，默认是最大
		AActor* ClosestActor; //缓存当前最近距离的目标
		for(AActor* PotentialTarget : ActorsToCheck)
		{
			//获取目标和中心的距离
			const double Distance = (PotentialTarget->GetActorLocation() - Origin).Length();

			//比对当前计算的位置是否小于缓存的位置
			if(Distance < ClosestDistance)
			{
				//如果小于，将替换对应信息
				ClosestDistance = Distance;
				ClosestActor = PotentialTarget;
			}
		}
		
		ActorsToCheck.Remove(ClosestActor); //从遍历数组中删除缓存的对象
		OutClosestTargets.AddUnique(ClosestActor); //添加到返回的数组中
		++ NumTargetFound; //递增数量
	}
}
