// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/AbilitySystemComponentBase.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "GameplayTagContainer.h"
#include "LPGameplayTags.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "AbilitySystem/Abilities/GameplayAbilityBase.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "Core/LoadScreenSaveGame.h"
#include "Interface/PlayerInterface.h"
#include "LostPlace/LPLogChannels.h"

void UAbilitySystemComponentBase::AbilityActorInfoSet()
{
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this, &UAbilitySystemComponentBase::ClientEffectApplied);
}

void UAbilitySystemComponentBase::AddCharacterAbilitiesFormSaveData(ULoadScreenSaveGame* SaveGameData)
{
	for(const FSavedAbility& Data : SaveGameData->SavedAbilities)
	{
		const TSubclassOf<UGameplayAbility> LoadedAbilityClass = Data.GameplayAbility;

		FGameplayAbilitySpec LoadedAbilitySpec = FGameplayAbilitySpec(LoadedAbilityClass, Data.AbilityLevel);

		// LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilityInputTag); //设置技能激活输入标签
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilitySlot); //设置技能激活输入标签
		LoadedAbilitySpec.GetDynamicSpecSourceTags().AddTag(Data.AbilityStatus); //设置技能状态标签

		//主动技能的处理
		if(Data.AbilityType == FLPGameplayTags::Get().Abilities_Type_Offensive)
		{
			GiveAbility(LoadedAbilitySpec); //只应用不激活
		}
		//被动技能的处理
		else if(Data.AbilityType == FLPGameplayTags::Get().Abilities_Type_Passive)
		{
			GiveAbility(LoadedAbilitySpec); //只应用不激活
			//确保技能已经装配
			if(Data.AbilityStatus.MatchesTagExact(FLPGameplayTags::Get().Abilities_Status_Equipped))
			{
				
				// GiveAbilityAndActivateOnce(LoadedAbilitySpec); //应用技能并激活
				TryActivateAbility(LoadedAbilitySpec.Handle);
			}
		}
	}

	bStartupAbilitiesGiven = true;
	AbilityGivenDelegate.Broadcast();
}

void UAbilitySystemComponentBase::AddCharacterAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		if (const UGameplayAbilityBase* AbilityBase = Cast<UGameplayAbilityBase>(AbilitySpec.Ability))
		{
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(AbilityBase->StartupInputTag);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FLPGameplayTags::Get().Abilities_Status_Equipped);
			GiveAbility(AbilitySpec);
		}
		bStartupAbilitiesGiven = true;
		AbilityGivenDelegate.Broadcast();
		// GiveAbility(AbilitySpec);
		// GiveAbilityAndActivateOnce(AbilitySpec);
	}
}

void UAbilitySystemComponentBase::AddCharacterPassiveAbilities(
	const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupPassiveAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		
		AbilitySpec.GetDynamicSpecSourceTags().AddTag(FLPGameplayTags::Get().Abilities_Status_Equipped);
		GiveAbilityAndActivateOnce(AbilitySpec);
	}
}
void UAbilitySystemComponentBase::AbilityInputTagPressed(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;

	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{

			AbilitySpecInputPressed(AbilitySpec);
			if (AbilitySpec.IsActive())
			{
				FPredictionKey PredictionKey;
				if (UGameplayAbility* AbilityInstance = AbilitySpec.GetPrimaryInstance())
				{
					PredictionKey = AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
				}else
				{
					TArray<UGameplayAbility*> Abilities = AbilitySpec.GetAbilityInstances();
					if (Abilities.Num() > 0)
					{
						PredictionKey = Abilities[0]->GetCurrentActivationInfo().GetActivationPredictionKey();
					}
				}
				// FPredictionKey PredictionKey = AbilitySpec.Ability.Get()->GetCurrentActivationInfo().GetActivationPredictionKey();
				InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputPressed, AbilitySpec.Handle, PredictionKey);
			}
		}
	}
	
}

void UAbilitySystemComponentBase::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
		{

			AbilitySpecInputPressed(AbilitySpec);
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
	
}

void UAbilitySystemComponentBase::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec);
			FPredictionKey PredictionKey;
			if (UGameplayAbility* AbilityInstance = AbilitySpec.GetPrimaryInstance())
			{
				PredictionKey = AbilityInstance->GetCurrentActivationInfo().GetActivationPredictionKey();
			}else
			{
				TArray<UGameplayAbility*> Abilities = AbilitySpec.GetAbilityInstances();
				if (Abilities.Num() > 0)
				{
					PredictionKey = Abilities[0]->GetCurrentActivationInfo().GetActivationPredictionKey();
				}
			}
			// FPredictionKey PredictionKey = AbilitySpec.Ability.Get()->GetCurrentActivationInfo().GetActivationPredictionKey();
			InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, 
								AbilitySpec.Handle, 
								PredictionKey);
		}
	}
}

void UAbilitySystemComponentBase::ForEachAbility(const FForEachAbility& Delegate)
{
	FScopedAbilityListLock ActiveScopeLock(*this); //使用域锁将此作用域this的内容锁定（无法修改），在遍历结束时解锁，保证线程安全
	for(const FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(!Delegate.ExecuteIfBound(AbilitySpec)) //运行绑定在技能实例上的委托，如果失败返回false
		{
			UE_LOG(LogLostPlace, Error, TEXT("在函数[%hs]运行委托失败"), __FUNCTION__);
		}
	}
}

FGameplayTag UAbilitySystemComponentBase::GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	if(AbilitySpec.Ability)
	{
		for(FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags) //获取设置的所有的技能标签并遍历
		{
			if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities")))) //判断当前标签是否包含"Abilities"名称
			{
				return Tag;
			}
		}
	}
	return FGameplayTag();
}

FGameplayTag UAbilitySystemComponentBase::GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags()) //从技能实例的动态标签容器中遍历所有标签
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("InputTag")))) //查找标签中是否设置以输入标签开头的标签
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayTag UAbilitySystemComponentBase::GetStatusFromSpec(const FGameplayAbilitySpec& AbilitySpec)
{
	for(FGameplayTag Tag : AbilitySpec.GetDynamicSpecSourceTags()) //从技能实例的动态标签容器中遍历所有标签
	{
		if(Tag.MatchesTag(FGameplayTag::RequestGameplayTag(FName("Abilities.Status")))) 
		{
			return Tag;
		}
	}

	return FGameplayTag();
}

FGameplayTag UAbilitySystemComponentBase::GetSlotFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetInputTagFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

FGameplayTag UAbilitySystemComponentBase::GetStatusFromAbilityTag(const FGameplayTag& AbilityTag)
{
	if (const FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		return GetStatusFromSpec(*AbilitySpec);
	}
	return FGameplayTag();
}

bool UAbilitySystemComponentBase::SlotIsEmpty(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);//域锁,遍历所有技能实例的时候都需要锁定
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilityHasSlot(AbilitySpec, Slot))
		{
			return false;
		}
	}
	return true;
}

void UAbilitySystemComponentBase::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	//判断Avatar是否基础角色接口
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		//判断是否用于可分配点数
		if(IPlayerInterface::Execute_GetAttributePoints(GetAvatarActor()) > 0)
		{
			ServerUpgradeAttribute(AttributeTag); //调用服务器升级属性
		}
	}
}

void UAbilitySystemComponentBase::UpdateAttribute(const FGameplayTag& AttributeTag, float Value)
{
	//判断Avatar是否基础角色接口
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		ServerUpdateAttribute(AttributeTag,Value); //调用服务器更新属性值
	}
}

void UAbilitySystemComponentBase::ServerUpdateAttribute_Implementation(const FGameplayTag& AttributeTag, float Value)
{
	FGameplayEventData Payload; //创建一个事件数据
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = Value;
	//向自身发送事件，通过被动技能接收属性加点
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
}

FGameplayAbilitySpec* UAbilitySystemComponentBase::GetSpecFromAbilityTag(const FGameplayTag& AbilityTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this); //域锁
	//遍历已经应用的技能
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		for(FGameplayTag Tag : AbilitySpec.Ability.Get()->AbilityTags)
		{
			if(Tag.MatchesTag(AbilityTag))
			{
				return &AbilitySpec;
			}
		}
	}
	return nullptr;
}

void UAbilitySystemComponentBase::UpdateAbilityStatuses(int32 Level)
{
	//从GameMode获取到技能配置数据
	UAbilityInfo* AbilityInfo = ULPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	for(const FLPAbilityInfo& Info : AbilityInfo->AbilityInformation)
	{
		if(!Info.AbilityTag.IsValid()) continue; //如果没有技能标签，取消执行
		if(Level < Info.LevelRequirement) continue; //如果当期等级未达到所需等级，取消执行
		//判断ASC中是否已存在当前技能实例
		if(GetSpecFromAbilityTag(Info.AbilityTag) == nullptr)
		{
			//如果没有技能实例，将应用一个新的技能实例
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(Info.Ability, 1);
			AbilitySpec.GetDynamicSpecSourceTags().AddTag(FLPGameplayTags::Get().Abilities_Status_Eligible);
			GiveAbility(AbilitySpec);
			MarkAbilitySpecDirty(AbilitySpec); //设置当前技能立即复制到每个客户端

			ClientUpdateAbilityStatus(Info.AbilityTag, FLPGameplayTags::Get().Abilities_Status_Eligible,1); //调用客户端更新技能状态
		}
	}
}

void UAbilitySystemComponentBase::ServerEquipAbility_Implementation(const FGameplayTag& AbilityTag, const FGameplayTag& Slot)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		const FLPGameplayTags& GameplayTags = FLPGameplayTags::Get();
		const FGameplayTag PrevSlot = GetInputTagFromSpec(*AbilitySpec);
		const FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		const bool bStatusValid = Status == GameplayTags.Abilities_Status_Equipped || Status == GameplayTags.Abilities_Status_Unlocked;
		
		if(bStatusValid)
		{
			//判断插槽是否有技能，有则需要将其清除
			if(!SlotIsEmpty(Slot))
			{
				//获取目标插槽现在装配的技能
				if( FGameplayAbilitySpec* SpecWithSlot = GetSpecWithSlot(Slot))
				{
					//技能槽位装配相同的技能，直接返回，不做额外的处理
					if(AbilityTag.MatchesTagExact(GetAbilityTagFromSpec(*SpecWithSlot)))
					{
						ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot);
						return;
					}

					// ClearAbilitiesOfSlot(Slot); //清除目标插槽装配的技能
					//如果是被动技能，我们需要先将技能取消执行
					if(IsPassiveAbility(*SpecWithSlot))
					{
						SpecWithSlot->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*SpecWithSlot));
						SpecWithSlot->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
						MulticastActivatePassiveEffect(GetAbilityTagFromSpec(*SpecWithSlot),false);
						DeactivatePassiveAbility.Broadcast(GetAbilityTagFromSpec(*SpecWithSlot));
					}
					ClearSlot(SpecWithSlot);
					
				}
			}

			//技能没有设置到插槽（没有激活）
			if(!AbilityHasAnySlot(*AbilitySpec))
			{
				//如果是被动技能，装配即激活
				if(IsPassiveAbility(*AbilitySpec))
				{
					TryActivateAbility(AbilitySpec->Handle);
					MulticastActivatePassiveEffect(AbilityTag,true);
				}
				AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GetStatusFromSpec(*AbilitySpec));
				AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
			}
			AssignSlotToAbility(*AbilitySpec,Slot); //设置技能槽位
			MarkAbilitySpecDirty(*AbilitySpec); //设置当前技能立即复制到每个客户端
			
		}
		ClientEquipAbility(AbilityTag, GameplayTags.Abilities_Status_Equipped, Slot, PrevSlot); //调用客户端装备技能
	}
}

void UAbilitySystemComponentBase::ClientEquipAbility_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& Status, const FGameplayTag& Slot, const FGameplayTag& PreviousSlot)
{
	AbilityEquipped.Broadcast(AbilityTag, Status, Slot, PreviousSlot);
}

bool UAbilitySystemComponentBase::GetDescriptionByAbilityTag(const FGameplayTag& AbilityTag, FString& OutDescription,
                                                             FString& OutNextLevelDescription)
{
	//如果当前技能处于锁定状态，将无法获取到对应的技能描述
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{
		if(UGameplayAbilityBase* LPAbility = Cast<UGameplayAbilityBase>(AbilitySpec->Ability))
		{
			OutDescription = LPAbility->GetDescription(AbilitySpec->Level);
			OutNextLevelDescription = LPAbility->GetNextLevelDescription(AbilitySpec->Level + 1);
			return true;
		}
	}

	//如果技能是锁定状态，将显示锁定技能描述
	UAbilityInfo* AbilityInfo = ULPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	if (!AbilityTag.IsValid() || AbilityTag.MatchesTagExact(FLPGameplayTags::Get().Abilities_None))
	{
		OutDescription = FString();
	}else
	{
		OutDescription = UGameplayAbilityBase::GetLockedDescription(AbilityInfo->FindAbilityInfoForTag(AbilityTag).LevelRequirement);
	}
	OutNextLevelDescription = FString();
	return  false;
}

void UAbilitySystemComponentBase::ClearSlot(FGameplayAbilitySpec* AbilitySpec)
{
	const FGameplayTag Slot = GetInputTagFromSpec(*AbilitySpec);
	AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(Slot);
}

void UAbilitySystemComponentBase::ClearAbilitiesOfSlot(const FGameplayTag& SlotTag)
{
	FScopedAbilityListLock ActiveScopeLoc(*this); //域锁
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilityHasSlot(AbilitySpec,SlotTag))
		{
			ClearSlot(&AbilitySpec);
		}
	}
}

bool UAbilitySystemComponentBase::AbilityHasSlot(FGameplayAbilitySpec& AbilitySpec, const FGameplayTag& SlotTag)
{
	return AbilitySpec.GetDynamicSpecSourceTags().HasTagExact(SlotTag);
}

bool UAbilitySystemComponentBase::AbilityHasAnySlot(FGameplayAbilitySpec& AbilitySpec)
{
	return AbilitySpec.GetDynamicSpecSourceTags().HasTag(FGameplayTag::RequestGameplayTag(FName("InputTag")));
}

FGameplayAbilitySpec* UAbilitySystemComponentBase::GetSpecWithSlot(const FGameplayTag& Slot)
{
	FScopedAbilityListLock ActiveScopeLoc(*this);
	for(FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if(AbilityHasSlot(AbilitySpec,Slot))
		{
			return &AbilitySpec;
		}
	}
	return nullptr;
}

bool UAbilitySystemComponentBase::IsPassiveAbility(const FGameplayAbilitySpec& Spec) const
{
	//从技能配置数据里获取到技能对于的配置信息
	UAbilityInfo* AbilityInfo = ULPAbilitySystemLibrary::GetAbilityInfo(GetAvatarActor());
	const FGameplayTag AbilityTag = GetAbilityTagFromSpec(Spec);
	const FLPAbilityInfo& Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);
	//判断信息里配置的技能类型是否为被动技能
	const FGameplayTag AbilityType = Info.AbilityType;
	return AbilityType.MatchesTagExact(FLPGameplayTags::Get().Abilities_Type_Passive);
}

void UAbilitySystemComponentBase::AssignSlotToAbility(FGameplayAbilitySpec& Spec, const FGameplayTag& Slot)
{
	const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
	ClearSlot(&Spec);
	Spec.GetDynamicSpecSourceTags().AddTag(Slot);
	Spec.GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Unlocked);
	Spec.GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Equipped);
}

void UAbilitySystemComponentBase::MulticastActivatePassiveEffect_Implementation(const FGameplayTag& AbilityTag,bool bActivate)
{
	ActivatePassiveEffect.Broadcast(AbilityTag, bActivate);
}

void UAbilitySystemComponentBase::ServerSpendSpellPoint_Implementation(const FGameplayTag& AbilityTag)
{
	if(FGameplayAbilitySpec* AbilitySpec = GetSpecFromAbilityTag(AbilityTag))
	{

		if (GetAvatarActor()->Implements<UPlayerInterface>())
		{
			IPlayerInterface::Execute_AddToSpellPoints(GetAvatarActor(), -1);
		}
		
		const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
		FGameplayTag Status = GetStatusFromSpec(*AbilitySpec);
		if(Status.MatchesTag(GameplayTags.Abilities_Status_Eligible))
		{
			AbilitySpec->GetDynamicSpecSourceTags().RemoveTag(GameplayTags.Abilities_Status_Eligible);
			AbilitySpec->GetDynamicSpecSourceTags().AddTag(GameplayTags.Abilities_Status_Unlocked);
			Status = GameplayTags.Abilities_Status_Unlocked;
		}
		else if (Status.MatchesTag(GameplayTags.Abilities_Status_Equipped)||Status.MatchesTag(GameplayTags.Abilities_Status_Unlocked))
		{
			AbilitySpec->Level += 1;
		}
		
		ClientUpdateAbilityStatus(AbilityTag,Status,AbilitySpec->Level);
		MarkAbilitySpecDirty(*AbilitySpec);
	}
}

void UAbilitySystemComponentBase::ServerUpgradeAttribute_Implementation(const FGameplayTag& AttributeTag)
{
	FGameplayEventData Payload; //创建一个事件数据
	Payload.EventTag = AttributeTag;
	Payload.EventMagnitude = 1.f;
	//向自身发送事件，通过被动技能接收属性加点
	UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(GetAvatarActor(), AttributeTag, Payload);
	//判断Avatar是否基础角色接口
	if(GetAvatarActor()->Implements<UPlayerInterface>())
	{
		IPlayerInterface::Execute_AddToAttributePoints(GetAvatarActor(), -1); //减少一点可分配属性点
	}
	
}

void UAbilitySystemComponentBase::ClientUpdateAbilityStatus_Implementation(const FGameplayTag& AbilityTag,
	const FGameplayTag& StatusTag,int32 AbilityLevel)
{
	AbilityStatusChanged.Broadcast(AbilityTag, StatusTag,AbilityLevel);
}

void UAbilitySystemComponentBase::OnRep_ActivateAbilities()
{
	Super::OnRep_ActivateAbilities();
	if(!bStartupAbilitiesGiven)
	{
		bStartupAbilitiesGiven = true;
		AbilityGivenDelegate.Broadcast();
	}
}

void UAbilitySystemComponentBase::ClientEffectApplied_Implementation(UAbilitySystemComponent* AbilitySystemComponent,
                                                                     const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle)
{
	FGameplayTagContainer TagsContainer;
	EffectSpec.GetAllAssetTags(TagsContainer);
	EffectAssetTags.Broadcast(TagsContainer);
}
