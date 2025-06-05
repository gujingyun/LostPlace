// Fill out your copyright notice in the Description page of Project Settings.


#include "LostPlace/Public/Actors/Bullet.h"

#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"

// Sets default values
ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);
	// 创建碰撞组件
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->InitSphereRadius(5.0f);
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	RootComponent = CollisionComponent;

	CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OnOverlapBegin);
	// CollisionComponent->OnComponentHit.AddDynamic(this, &ABullet::OnHit);
	// 可选 Mesh
	BulletMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletMesh"));
	BulletMesh->SetupAttachment(RootComponent);
	BulletMesh->SetRelativeScale3D(FVector(0.2f));

	// 创建移动组件
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = BulletSpeed;
	ProjectileMovement->MaxSpeed = BulletSpeed;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f; // 关键：设置为 0 不受重力影响
	ProjectileMovement->bShouldBounce = false;         // （可选）禁用弹跳

	// 子弹生命周期
	InitialLifeSpan = 3.0f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();
	if (AActor* MyOwner = GetOwner())
	{
		CollisionComponent->IgnoreActorWhenMoving(MyOwner, true);
	}
}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
void ABullet::FireInDirection(const FVector& ShootDirection)
{
	ProjectileMovement->Velocity = ShootDirection.GetSafeNormal() * ProjectileMovement->InitialSpeed;
	// 忽略和发射者（Instigator）发生碰撞
	if (GetInstigator())
	{
		CollisionComponent->IgnoreActorWhenMoving(GetInstigator(), true);
	}
}

void ABullet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// //不用了
// void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
// 					  UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
// {
// 	// 忽略自身或无效碰撞
// 	if (OtherActor && OtherActor != this && OtherComp)
// 	{
// 		if (OtherActor == GetInstigator()) return;
// 		UHealthComponent* HealthComp = OtherActor->FindComponentByClass<UHealthComponent>();
// 		if (HealthComp)
// 		{
// 			// 对敌人造成伤害，例如：造成 20 点伤害
// 			HealthComp->TakeDamage(BulletDamage);
// 		}
// 		// 销毁子弹
// 		Destroy();
// 	}
// }
void ABullet::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}
	if (!bCanPenetrate)
	{
		Destroy();
	}
}