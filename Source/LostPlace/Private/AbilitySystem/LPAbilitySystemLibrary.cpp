// Fill out your copyright notice in the Description page of Project Settings.
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "UI/WidgetController/OverlayWidgetController.h"
#include "UI/WidgetController/AttributeMenuWidgetController.h"
#include "LPAbilityTypes.h"
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
