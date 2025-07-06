// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MagicCircle.generated.h"

UCLASS()
class LOSTPLACE_API AMagicCircle : public AActor
{
	GENERATED_BODY()

public:

	AMagicCircle();

	virtual void Tick(float DeltaTime) override;

	//魔法光环法阵贴花
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TObjectPtr<UDecalComponent> MagicCircleDecal;
protected:
	virtual void BeginPlay() override;
	


};
