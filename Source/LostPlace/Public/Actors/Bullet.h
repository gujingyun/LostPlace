// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

class UProjectileMovementComponent;
class USphereComponent;

UCLASS()
class LOSTPLACE_API ABullet : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ABullet();
	// 设置子弹方向（从外部调用）
	void FireInDirection(const FVector& ShootDirection);
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	// 碰撞处理函数
	// UFUNCTION()
	// void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	// 		   UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
						UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	// 碰撞组件
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionComponent;

	// 移动组件
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// 可选：可视化 Mesh
	UPROPERTY(VisibleAnywhere, Category = "Visual")
	UStaticMeshComponent* BulletMesh;

	// 子弹速度
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "子弹属性")
	float BulletSpeed = 1000.0f;	// 1000.0f 是一个示例值，你可以根据需要调整
	// 子弹伤害
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "子弹属性")
	float BulletDamage = 10.0f;	// 10.0f 是一个示例值，你可以根据需要调整
	
	// 是否使用重叠（Overlap）来处理伤害；否则使用命中（Hit）
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "子弹属性")
	bool bCanPenetrate = false;
};
