// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LPAbilitySystemLibrary.generated.h"
struct FWidgetControllerParams;
class USpellMenuWidgetController;
class UOverlayWidgetController;
class UAttributeMenuWidgetController;
class ALPHUD;
class UAbilityInfo;
/**
 * 
 */
UCLASS()
class LOSTPLACE_API ULPAbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintPure, Category = "LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static bool MakeOverlayWidgetControllerPrParams(const UObject* WorldContextObject,FWidgetControllerParams& OutWCParams,ALPHUD*& OutHUD);
	
	UFUNCTION(BlueprintPure, Category = "LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static UOverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|WidgetController",meta=(DefaultToSelf="WorldContextObject"))
	static USpellMenuWidgetController* GetSpellMenuWidgetController(const UObject* WorldContextObject);

	//初始化角色的属性
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);

	//初始化角色的技能
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|AbilityInfo")
	static UAbilityInfo* GetAbilityInfo(const UObject* WorldContextObject);

	
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsBlockedHit);

	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bIsCriticalHit);

	//获取到攻击位置半径内的所有动态Actor
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);

	//获取双方是否为敌对势力
	UFUNCTION(BlueprintPure, Category="LPAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);


	//获取根据敌人类型和等级获取敌人产生的经验
	UFUNCTION(BlueprintCallable, Category="LPAbilitySystemLibrary|CharacterClassDefaults")
	static int32 GetXPRewardForClassAndLevel(const UObject* WorldContextObject, ECharacterClass CharacterClass, int32 CharacterLevel);

};
