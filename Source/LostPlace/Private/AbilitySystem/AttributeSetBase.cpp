// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AttributeSetBase.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "GameFramework/Character.h"
#include "Net/UnrealNetwork.h"
#include "LPGameplayTags.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "Interface/CombatInterface.h"
#include "Interface/PlayerInterface.h"
#include "Kismet/GameplayStatics.h"
#include "LostPlace/LPLogChannels.h"
#include "Player/LPPlayerController.h"

UAttributeSetBase::UAttributeSetBase()
{
	const FLPGameplayTags& GameplayTags = FLPGameplayTags::Get();

	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Strength, GetStrengthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Intelligence, GetIntelligenceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Resilience, GetResilienceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Primary_Vigor, GetVigorAttribute);

	
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_Armor, GetArmorAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ArmorPenetration, GetArmorPenetrationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_BlockChance, GetBlockChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitChance, GetCriticalHitChanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitDamage, GetCriticalHitDamageAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_CriticalHitResistance, GetCriticalHitResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_HealthRegeneration, GetHealthRegenerationAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_ManaRegeneration, GetManaRegenerationAttribute);

	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxHealth, GetMaxHealthAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Secondary_MaxMP, GetMaxMPAttribute);

	/*
	 * Resistance Attribute
	 */
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Fire, GetFireResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Lightning, GetLightningResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Arcane, GetArcaneResistanceAttribute);
	TagsToAttributes.Add(GameplayTags.Attributes_Resistance_Physical, GetPhysicalResistanceAttribute);
	
}

void UAttributeSetBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//主要属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Strength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Intelligence, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Resilience, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Vigor, COND_None, REPNOTIFY_Always);

	//次级属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, MaxMP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, Armor, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, ArmorPenetration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, BlockChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, CriticalHitChance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, CriticalHitDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, CriticalHitResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, HealthRegeneration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, ManaRegeneration, COND_None, REPNOTIFY_Always);

	//抗性属性
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, FireResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, LightningResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, ArcaneResistance, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UAttributeSetBase, PhysicalResistance, COND_None, REPNOTIFY_Always);

}

void UAttributeSetBase::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);
	if(Attribute == GetHealthAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxHealth());
	}
	if(Attribute == GetMPAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.f, GetMaxMP());
	}

}

void UAttributeSetBase::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);
	FEffectProperties Props;
	SetEffectProperties(Data, Props);
	if(Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		SetHealth(FMath::Clamp(GetHealth(), 0.f, GetMaxHealth()));
	}
	if(Data.EvaluatedData.Attribute == GetMPAttribute())
	{
		SetMP(FMath::Clamp(GetMP(), 0.f, GetMaxMP()));
	}
	if (Data.EvaluatedData.Attribute == GetIncomingDamageAttribute())
	{
		const float LocalIncomingDamage = GetIncomingDamage();
		SetIncomingDamage(0.f);
		if (LocalIncomingDamage > 0.f)
		{
			const float NewHealth = GetHealth() - LocalIncomingDamage;
			SetHealth(FMath::Clamp(NewHealth, 0.f, GetMaxHealth()));

			const bool bFatal = NewHealth <= 0.f;
			if(bFatal)
			{
				//调用死亡函数
				ICombatInterface* CombatInterface = Cast<ICombatInterface>(Props.TargetAvatarActor);
				if(CombatInterface)
				{
					CombatInterface->Die();
				}
				SendXPEvent(Props);
			}
			else
			{
				FGameplayTagContainer TagContainer;
				TagContainer.AddTag(FLPGameplayTags::Get().Effects_HitReact);
				Props.TargetASC->TryActivateAbilitiesByTag(TagContainer); //根据tag标签激活技能
			}
			const bool bBlockedHit = ULPAbilitySystemLibrary::IsBlockedHit(Props.EffectContextHandle);
			const bool bCriticalHit = ULPAbilitySystemLibrary::IsCriticalHit(Props.EffectContextHandle);
			ShowFloatingText(Props, LocalIncomingDamage,bBlockedHit,bCriticalHit);
		}
	}

	if(Data.EvaluatedData.Attribute == GetIncomingXPAttribute())
	{
		const float LocalIncomingXP = GetIncomingXP();
		SetIncomingXP(0.f);
		//将经验应用给自身
		if(Props.SourceCharacter->Implements<UPlayerInterface>() && Props.SourceCharacter->Implements<UCombatInterface>())
		{
			const int32 CurrentLevel = ICombatInterface::Execute_GetPlayerLevel(Props.SourceCharacter);
			const int32 CurrentXP = IPlayerInterface::Execute_GetXP(Props.SourceCharacter);

			const int32 NewLevel = IPlayerInterface::Execute_FindLevelForXP(Props.SourceCharacter,CurrentXP+LocalIncomingXP);
			const int32 NumLevelUps = NewLevel - CurrentLevel;
			if(NumLevelUps > 0)
			{
				//获取升级提供的技能点和属性点
				int32 AttributePointsReward = IPlayerInterface::Execute_GetAttributePointsReward(Props.SourceCharacter, CurrentLevel);
				int32 SpellPointsReward = IPlayerInterface::Execute_GetSpellPointsReward(Props.SourceCharacter, CurrentLevel);

				//提升等级，增加角色技能点和属性点
				IPlayerInterface::Execute_AddToPlayerLevel(Props.SourceCharacter, NumLevelUps);
				IPlayerInterface::Execute_AddToAttributePoints(Props.SourceCharacter, AttributePointsReward);
				IPlayerInterface::Execute_AddToSpellPoints(Props.SourceCharacter, SpellPointsReward);

				IPlayerInterface::Execute_LevelUp(Props.SourceCharacter); //升级

				//将血量和蓝量填充满
				SetHealth(GetMaxHealth());
				SetMP(GetMaxMP());
			}
			
			IPlayerInterface::Execute_AddToXP(Props.SourceCharacter, LocalIncomingXP);
		}
	}
}
void UAttributeSetBase::SendXPEvent(const FEffectProperties& Props)
{
	if(Props.SourceCharacter->Implements<UPlayerInterface>())
	{
		//从战斗接口获取等级和职业，通过蓝图函数获取可提供的经验值
		const int32 TargetLevel = ICombatInterface::Execute_GetPlayerLevel(Props.TargetCharacter);
		const ECharacterClass TargetClass = ICombatInterface::Execute_GetCharacterClass(Props.TargetCharacter); //c++内调用BlueprintNativeEvent函数需要这样调用
		const int32 XPReward = ULPAbilitySystemLibrary::GetXPRewardForClassAndLevel(Props.TargetCharacter, TargetClass, TargetLevel);

		const FLPGameplayTags& GameplayTags = FLPGameplayTags::Get();
		FGameplayEventData Payload; //创建Payload
		Payload.EventTag = GameplayTags.Attributes_Meta_IncomingXP;
		Payload.EventMagnitude = XPReward;
		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Props.SourceCharacter, GameplayTags.Attributes_Meta_IncomingXP, Payload);
	}
}
void UAttributeSetBase::ShowFloatingText(const FEffectProperties& Props, float Damage,bool bBlockedHit,bool bCriticalHit) const
{
	//调用显示伤害数字
	if(Props.SourceCharacter != Props.TargetCharacter)
	{
		if(ALPPlayerController* PC = Cast<ALPPlayerController>(Props.SourceCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter,bBlockedHit,bCriticalHit); //调用显示伤害数字
		}
		if(ALPPlayerController* PC = Cast<ALPPlayerController>(Props.TargetCharacter->Controller))
		{
			PC->ShowDamageNumber(Damage, Props.TargetCharacter,bBlockedHit,bCriticalHit); //调用显示伤害数字
		}
	}
}
void UAttributeSetBase::OnRep_Strength(const FGameplayAttributeData& OldStrength) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Strength, OldStrength);
}

void UAttributeSetBase::OnRep_Intelligence(const FGameplayAttributeData& OldIntelligence) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Intelligence, OldIntelligence);
}

void UAttributeSetBase::OnRep_Resilience(const FGameplayAttributeData& OldResilience) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Resilience, OldResilience);
}

void UAttributeSetBase::OnRep_Vigor(const FGameplayAttributeData& OldVigor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Vigor, OldVigor);
}



void UAttributeSetBase::OnRep_Health(const FGameplayAttributeData& OldHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Health, OldHealth);
}

void UAttributeSetBase::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxHealth, OldMaxHealth);
}
void UAttributeSetBase::OnRep_MP(const FGameplayAttributeData& OldMP) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MP, OldMP);
}

void UAttributeSetBase::OnRep_MaxMP(const FGameplayAttributeData& OldMaxMP) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, MaxMP, OldMaxMP);
}

void UAttributeSetBase::OnRep_Armor(const FGameplayAttributeData& OldArmor) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, Armor, OldArmor);
}

void UAttributeSetBase::OnRep_ArmorPenetration(const FGameplayAttributeData& OldArmorPenetration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, ArmorPenetration, OldArmorPenetration);
}

void UAttributeSetBase::OnRep_BlockChance(const FGameplayAttributeData& OldBlockChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, BlockChance, OldBlockChance);
}

void UAttributeSetBase::OnRep_CriticalHitChance(const FGameplayAttributeData& OldCriticalHitChance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, CriticalHitChance, OldCriticalHitChance);
}

void UAttributeSetBase::OnRep_CriticalHitDamage(const FGameplayAttributeData& OldCriticalHitDamage) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, CriticalHitDamage, OldCriticalHitDamage);
}

void UAttributeSetBase::OnRep_CriticalHitResistance(const FGameplayAttributeData& OldCriticalHitResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, CriticalHitResistance, OldCriticalHitResistance);
}

void UAttributeSetBase::OnRep_HealthRegeneration(const FGameplayAttributeData& OldHealthRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, HealthRegeneration, OldHealthRegeneration);
}

void UAttributeSetBase::OnRep_ManaRegeneration(const FGameplayAttributeData& OldManaRegeneration) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, ManaRegeneration, OldManaRegeneration);
}

void UAttributeSetBase::OnRep_FireResistance(const FGameplayAttributeData& OldFireResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, FireResistance, OldFireResistance);
}

void UAttributeSetBase::OnRep_LightningResistance(const FGameplayAttributeData& OldLightningResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, LightningResistance, OldLightningResistance);
}

void UAttributeSetBase::OnRep_ArcaneResistance(const FGameplayAttributeData& OldArcaneResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, ArcaneResistance, OldArcaneResistance);
}

void UAttributeSetBase::OnRep_PhysicalResistance(const FGameplayAttributeData& OldPhysicalResistance) const
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UAttributeSetBase, PhysicalResistance, OldPhysicalResistance);
}


void UAttributeSetBase::SetEffectProperties(const FGameplayEffectModCallbackData& Data,FEffectProperties& Props) const
{
	//Source 效果的所有者   Target 效果应用的目标
	Props.EffectContextHandle = Data.EffectSpec.GetContext();
	Props.SourceASC = Props.EffectContextHandle.GetOriginalInstigatorAbilitySystemComponent(); //获取效果所有者的ASC
	//获取效果所有者的相关对象
	if(IsValid(Props.SourceASC) && Props.SourceASC->AbilityActorInfo.IsValid() && Props.SourceASC->AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.SourceAvatarActor = Props.SourceASC->AbilityActorInfo->AvatarActor.Get(); //获取Actor
		Props.SourceController = Props.SourceASC->AbilityActorInfo->PlayerController.Get(); //获取PlayerController
		if(Props.SourceController == nullptr && Props.SourceAvatarActor != nullptr)
		{
			if(const APawn* Pawn = Cast<APawn>(Props.SourceAvatarActor))
			{
				Props.SourceController = Pawn->GetController();
			}
		}

		if(Props.SourceController)
		{
			Props.SourceCharacter = Cast<ACharacter>(Props.SourceController->GetPawn());
		}
	}

	if(Data.Target.AbilityActorInfo.IsValid() && Data.Target.AbilityActorInfo->AvatarActor.IsValid())
	{
		Props.TargetAvatarActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		Props.TargetController = Data.Target.AbilityActorInfo->PlayerController.Get();
		Props.TargetCharacter = Cast<ACharacter>(Props.TargetAvatarActor);
		Props.TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(Props.TargetAvatarActor);
	}
}

