// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "LPAIController.generated.h"

class UBlackboardComponent;
class UBehaviorTreeComponent;
/**
 *
 */
UCLASS()
class LOSTPLACE_API ALPAIController : public AAIController
{
	GENERATED_BODY()

public:
	ALPAIController();

protected:
	
	UPROPERTY()
	TObjectPtr<UBehaviorTreeComponent> BehaviorTreeComponent;

};
