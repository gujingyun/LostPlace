// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/CharacterBase.h"
#include "AbilitySystemComponent.h"
#include "LPGameplayTags.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "LostPlace/LostPlace.h"
// Sets default values
ACharacterBase::ACharacterBase()
{
	PrimaryActorTick.bCanEverTick = false;

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); //设置可视为阻挡
	GetCapsuleComponent()->SetGenerateOverlapEvents(false);
	GetMesh()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore); 
	GetMesh()->SetCollisionResponseToChannel(ECC_Projectile,ECR_Overlap); 
	GetMesh()->SetGenerateOverlapEvents(true); 
	
	Weapon = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponComponent"));//创建空间
	Weapon->SetupAttachment(GetMesh(), TEXT("WeaponHandSocket"));//设置武器的位置
	Weapon->SetCollisionEnabled(ECollisionEnabled::NoCollision);//设置武器的碰撞
	


}


UAnimMontage* ACharacterBase::GetHitReactMontage_Implementation()
{
	return HitReactMontage;
}

void ACharacterBase::Die()
{
	//将武器从角色身上分离
	Weapon->DetachFromComponent(FDetachmentTransformRules(EDetachmentRule::KeepWorld, true));
	MulticastHandleDeath();
}

FVector ACharacterBase::GetCombatSocketLocation_Implementation(const FGameplayTag MontageTag)
{
	const FLPGameplayTags GameplayTags = FLPGameplayTags::Get();
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Weapon)&&IsValid(Weapon))
	{
		return Weapon->GetSocketLocation(WeaponTipSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_LeftHand))
	{
		return GetMesh()->GetSocketLocation(LeftHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_RightHand))
	{
		return GetMesh()->GetSocketLocation(RightHandSocketName);
	}
	if (MontageTag.MatchesTagExact(GameplayTags.CombatSocket_Tail))
	{
		return GetMesh()->GetSocketLocation(TailSocketName);
	}
	return FVector();
}



void ACharacterBase::MulticastHandleDeath_Implementation()
{
	//播放死亡音效
	UGameplayStatics::PlaySoundAtLocation(this, DeathSound, GetActorLocation());
	//开启武器物理效果
	Weapon->SetSimulatePhysics(true); //开启模拟物理效果
	Weapon->SetEnableGravity(true); //开启重力效果
	Weapon->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); //开启物理碰撞通道

	//开启角色物理效果
	GetMesh()->SetSimulatePhysics(true); //开启模拟物理效果
	GetMesh()->SetEnableGravity(true); //开启重力效果
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::PhysicsOnly); //开启物理碰撞通道
	GetMesh()->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Block); //开启角色与静态物体产生碰撞

	//关闭角色碰撞体碰撞通道，避免其对武器和角色模拟物理效果产生影响
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Dissolve();
	bDead = true;
}


// Called when the game starts or when spawned
void ACharacterBase::BeginPlay()
{
	Super::BeginPlay();

	
}


bool ACharacterBase::IsDead_Implementation() const
{
	return bDead;
}

AActor* ACharacterBase::GetAvatar_Implementation()
{
	return this;
}

TArray<FTaggedMontage> ACharacterBase::GetAttackMontages_Implementation()
{
	return AttackMontage;
}

UAbilitySystemComponent* ACharacterBase::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void ACharacterBase::InitAbilityActorInfo()
{
}


void ACharacterBase::InitDefaultAttributes() const
{
	ApplyEffectToSelf(DefaultPrimaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultSecondaryAttributes, 1.f);
	ApplyEffectToSelf(DefaultVitalAttributes, 1.f);
}

void ACharacterBase::ApplyEffectToSelf(TSubclassOf<UGameplayEffect> GameplayEffectClass, float Level) const
{
	check(IsValid(GetAbilitySystemComponent()));
	check(GameplayEffectClass);
	FGameplayEffectContextHandle ContextHandle = GetAbilitySystemComponent()->MakeEffectContext();
	ContextHandle.AddSourceObject(this);
	const FGameplayEffectSpecHandle SpecHandle = GetAbilitySystemComponent()->MakeOutgoingSpec(GameplayEffectClass, Level, ContextHandle);
	GetAbilitySystemComponent()->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), GetAbilitySystemComponent());
	
}

void ACharacterBase::AddCharacterAbilities()
{
	UAbilitySystemComponentBase* LPASC = CastChecked<UAbilitySystemComponentBase>(AbilitySystemComponent);
	if (!HasAuthority()) return;
	
	LPASC->AddCharacterAbilities(StartupAbilities);
}

void ACharacterBase::Dissolve()
{
	TArray<UMaterialInstanceDynamic*> MatArray;
	//设置角色溶解
	if(IsValid(DissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(DissolveMaterialInstance, this);
		GetMesh()->SetMaterial(0, DynamicMatInst);
		MatArray.Add(DynamicMatInst);
	}

	//设置武器溶解
	if(IsValid(WeaponDissolveMaterialInstance))
	{
		UMaterialInstanceDynamic* DynamicMatInst = UMaterialInstanceDynamic::Create(WeaponDissolveMaterialInstance, this);
		Weapon->SetMaterial(0, DynamicMatInst);
		MatArray.Add(DynamicMatInst);
	}

	//调用时间轴渐变溶解
	StartDissolveTimeline(MatArray);
}

UNiagaraSystem* ACharacterBase::GetBloodEffect_Implementation()
{
	return BloodEffect;
}

FTaggedMontage ACharacterBase::GetTaggedMontageByTag_Implementation(const FGameplayTag& MontageTag)
{
	for(FTaggedMontage TaggedMontage : AttackMontage)
	{
		if(TaggedMontage.MontageTag.MatchesTagExact(MontageTag))
		{
			return TaggedMontage;
		}
	}
	return FTaggedMontage();
}

int32 ACharacterBase::GetMinionCount_Implementation()
{
	return MinionsCount;
}

void ACharacterBase::IncrementMinionCount_Implementation(const int32 Amount)
{
	MinionsCount+=Amount;
}

// Called every frame
void ACharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

