// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Abilities/LPSummonAbility.h"

TArray<FVector> ULPSummonAbility::GetSpawnLocations()
{
	const FVector Forward = GetAvatarActorFromActorInfo()->GetActorForwardVector(); //获取召唤师的朝向
	const FVector Location = GetAvatarActorFromActorInfo()->GetActorLocation(); //获取召唤师的位置
	const float DeltaSpread = SpawnSpread / (NumMinions - 1.f); //将召唤的角度范围进行分段，在每段里面生成一个召唤物

	const FVector LeftOfSpread = Forward.RotateAngleAxis(-SpawnSpread / 2.f, FVector::UpVector); //获取到最左侧的角度

	TArray<FVector> SpawnLocations;
	for(int32 i = 0; i < NumMinions; i++) //遍历，在每个分段上面获取位置
	{
		const FVector Direction = LeftOfSpread.RotateAngleAxis(DeltaSpread * i, FVector::UpVector); //获取当前分段的角度
		FVector ChosenSpawnLocation = Location + Direction * FMath::FRandRange(MinSpawnDistance, MaxSpawnDistance); //随机位置，加上原始位置就是偏移的位置

		FHitResult Hit;
		GetWorld()->LineTraceSingleByChannel(Hit, ChosenSpawnLocation + FVector(0.f, 0.f, 400.f), ChosenSpawnLocation - FVector(0.f, 0.f, 400.f), ECC_Visibility);
		if(Hit.bBlockingHit)
		{
			ChosenSpawnLocation = Hit.ImpactPoint;
		}
		SpawnLocations.Add(ChosenSpawnLocation);
	}

	return SpawnLocations;

}

TSubclassOf<APawn> ULPSummonAbility::GetRandomMinionClass()
{
	const int32 Selection = FMath::RandRange(0, MinionClasses.Num() - 1);
	return MinionClasses[Selection];
}
