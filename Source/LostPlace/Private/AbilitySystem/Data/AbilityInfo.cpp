// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AbilityInfo.h"

#include "LostPlace/LPLogChannels.h"

FLPAbilityInfo UAbilityInfo::FindAbilityInfoForTag(const FGameplayTag& AbilityTag, bool bLogNotFound) const
{
	for(const FLPAbilityInfo& Info : AbilityInformation)
	{
		if(Info.AbilityTag == AbilityTag)
		{
			return Info;
		}
	}

	if(bLogNotFound)
	{
		//如果获取不到数据，打印消息
		UE_LOG(LogLostPlace,Error,TEXT("在AbilityInfo类[%s]中，没有找到能力标签[%s]"),*GetNameSafe(this),*AbilityTag.ToString());
	}

	return FLPAbilityInfo();
}
