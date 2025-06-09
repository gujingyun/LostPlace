// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "AbilitySystem/Data/LevelUpInfo.h"
#include "GameFramework/PlayerState.h"
#include "LPPlayerState.generated.h"

class UAbilitySystemComponent;
class UAttributeSet;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnPlayerStateChanged, int32);

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ALPPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
public:
	ALPPlayerState();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override; //覆盖虚函数获取asc
	UAttributeSet* GetAttributeSet() const { return AttributeSet; } //获取as
	
	FORCEINLINE int32 GetPlayerLevel() const { return Level; }//获取角色等级
	FORCEINLINE int32 GetXP() const {return XP;} //获取角色当前经验值
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<ULevelUpInfo> LevelUpInfo; //设置升级相关数据
	
	FOnPlayerStateChanged OnXPChangedDelegate; //经验值变动委托
	FOnPlayerStateChanged OnLevelChangedDelegate; //等级变动委托
	FOnPlayerStateChanged OnAttributePointsChangedDelegate; //属性点数变动委托
	FOnPlayerStateChanged OnSpellPointsChangedDelegate; //技能点数变动委托
	void AddToLevel(int32 InLevel); //增加等级
	void SetLevel(int32 InLevel); //设置当前等级

	
	void AddToXP(int32 InXP); //增加经验值
	void SetXP(int32 InXP); //设置当前经验值

	//属性点
	FORCEINLINE int32 GetAttributePoints() const {return AttributePoints;} //获取角色当前属性点
	void AddToAttributePoints(int32 InAttributePoints); //增加属性点
	void SetAttributePoints(int32 InAttributePoints); //设置当前属性点
	//技能点
	FORCEINLINE int32 GetSpellPoints() const {return SpellPoints;} //获取角色当前技能点
	void AddToSpellPoints(int32 InSpellPoints); //增加技能点
	void SetSpellPoints(int32 InSpellPoints); //设置当前技能点

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
	UPROPERTY()
	TObjectPtr<UAttributeSet> AttributeSet;
private:
	UPROPERTY(VisibleAnywhere, ReplicatedUsing = OnRep_Level)
	int32 Level = 1;
	UFUNCTION()
	void OnRep_Level(int32 OldLevel) const;
	
	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_XP)
	int32 XP = 0;
	
	UFUNCTION()
	void OnRep_XP(int32 OldXP) const; //服务器出现更改自动同步到本地函数 经验值

	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_AttributePoints)
	int32 AttributePoints = 0;

	UFUNCTION()
	void OnRep_AttributePoints(int32 OldAttributePoints) const; //服务器出现更改自动同步到本地函数 属性点


	UPROPERTY(VisibleAnywhere, ReplicatedUsing=OnRep_SpellPoints)
	int32 SpellPoints = 1;

	UFUNCTION()
	void OnRep_SpellPoints(int32 OldSpellPoints) const; //服务器出现更改自动同步到本地函数 属性点

};
