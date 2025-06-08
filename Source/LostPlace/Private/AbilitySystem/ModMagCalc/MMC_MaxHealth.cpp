// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/ModMagCalc/MMC_MaxHealth.h"

#include "AbilitySystem/AttributeSetBase.h"
#include "Interface/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	VigorDef.AttributeToCapture = UAttributeSetBase::GetVigorAttribute(); //设置需要获取的属性对象
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target; //设置拾取对象为GE的应用目标
	VigorDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(VigorDef); //添加到捕获属性数值，只有添加到列表，才会去获取属性值
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//  从 source 和 target 获取 Tag
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	//获取体力值
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef, Spec, EvaluateParameters, Vigor);
	Vigor = FMath::Max<float>(Vigor, 0.f);

	//获取等级
	int32 PlayerLevel = 1;
	if(Spec.GetContext().GetSourceObject()->Implements<UCombatInterface>())
	{
		PlayerLevel = ICombatInterface::Execute_GetPlayerLevel(Spec.GetContext().GetSourceObject());
	}

	//计算最大血量
	return 80.f + Vigor * 2.5f + PlayerLevel * 10.f;
}

