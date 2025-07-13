// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interface/SaveInterface.h"
#include "LPEnemySpawnVolume.generated.h"

class UBoxComponent;
class ALPEnemySpawnPoint;

UCLASS()
class LOSTPLACE_API ALPEnemySpawnVolume : public AActor, public ISaveInterface
{
	GENERATED_BODY()

public:
	ALPEnemySpawnVolume();

	/* Save Interface */
	virtual void LoadActor_Implementation() override;
	/* Save Interface 结束 */

	//当前怪物生成体积是否已经生成过敌人
	UPROPERTY(BlueprintReadOnly, SaveGame)
	bool bReached = false;

protected:
	virtual void BeginPlay() override;
	
	UFUNCTION()
	virtual void OnBoxOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	//配置当前的生成体积所需生成的敌人
	UPROPERTY(EditAnywhere)
	TArray<ALPEnemySpawnPoint*> SpawnPoints;
private:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UBoxComponent> Box;
};
