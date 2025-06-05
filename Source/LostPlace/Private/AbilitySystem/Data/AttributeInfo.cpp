// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Data/AttributeInfo.h"

FLPAttributeInfo UAttributeInfo::FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for(const FLPAttributeInfo& Info : AttributeInformation)
	{
		//判断标签是否完全匹配
		if(Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	//未查询到是否打印
	if(bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("无法从数据列表[%s]中查找到对应的AttributeTag[%s]"), *GetNameSafe(this), *AttributeTag.ToString());
	}

	return FLPAttributeInfo();
}
