// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/LPEnemySpawnPoint.h"

#include "Characters/LPEnemyCharacter.h"


void ALPEnemySpawnPoint::SpawnEnemy()
{
	//延迟生成Actor，并设置其尝试调整位置但固定生成
	ALPEnemyCharacter* Enemy = GetWorld()->SpawnActorDeferred<ALPEnemyCharacter>(
		EnemyClass,
		GetActorTransform(),
		nullptr,
		nullptr,
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
	
	Enemy->SetLevel(EnemyLevel);
	Enemy->SetCharacterClass(CharacterClass);
	Enemy->FinishSpawning(GetActorTransform());
	Enemy->SpawnDefaultController();
}
