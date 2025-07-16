// Fill out your copyright notice in the Description page of Project Settings.


#include "LostPlace/Public/Characters/LPEnemyCharacter.h"

#include "LPGameplayTags.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/AttributeSetBase.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "AI/LPAIController.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "LostPlace/LostPlace.h"
#include "UI/LPUserWidget.h"


// Sets default values
ALPEnemyCharacter::ALPEnemyCharacter()
{
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	GetCharacterMovement()->bUseControllerDesiredRotation = true; 
	
	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent()); //将血条附件到根节点上
	
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	GetMesh()->MarkRenderStateDirty();
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED);
	Weapon->MarkRenderStateDirty();
	
	BaseWalkSpeed = 250.f;
}

void ALPEnemyCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if (!HasAuthority()) return;
	//设置控制器
	LPAIController = Cast<ALPAIController>(NewController);

	LPAIController->GetBlackboardComponent()->InitializeBlackboard(*BehaviorTree->BlackboardAsset);
	LPAIController->RunBehaviorTree(BehaviorTree);

	LPAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("HitReacting"), false);
	LPAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("RangedAttacker"), CharacterClass != ECharacterClass::Warrior);

}


// Called when the game starts or when spawned
void ALPEnemyCharacter::BeginPlay()
{
	Super::BeginPlay();
	//设置角色的初始移动速度
	GetCharacterMovement()->MaxWalkSpeed = BaseWalkSpeed;

	//初始化角色的ASC
	InitAbilityActorInfo();
	if(HasAuthority())
	{
		//初始化角色的技能
		ULPAbilitySystemLibrary::GiveStartupAbilities(this, AbilitySystemComponent,CharacterClass);
	}

	if(ULPUserWidget* UserWidget = Cast<ULPUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		UserWidget->SetWidgetController(this);
	}
	
	if(const UAttributeSetBase* AS = Cast<UAttributeSetBase>(AttributeSet))
	{
		//监听血量变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
		);
		AbilitySystemComponent->RegisterGameplayTagEvent(FLPGameplayTags::Get().Effects_HitReact, EGameplayTagEventType::NewOrRemoved).AddUObject(this, &ThisClass::HitReactTagChanged);

		//初始化血量
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}
}
void ALPEnemyCharacter::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting? 0.f : BaseWalkSpeed;
	if (LPAIController && LPAIController->GetBlackboardComponent())
	{
		LPAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("HitReacting"), bHitReacting);
	}
	
}

void ALPEnemyCharacter::InitAbilityActorInfo()
{
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	Cast<UAbilitySystemComponentBase>(AbilitySystemComponent)->AbilityActorInfoSet();
	if (HasAuthority())
	{
		InitDefaultAttributes();
	}
	OnASCRegistered.Broadcast(AbilitySystemComponent);
	//注册监听负面标签变动
	DeBuffRegisterChanged();
}

void ALPEnemyCharacter::InitDefaultAttributes() const
{
	ULPAbilitySystemLibrary::InitializeDefaultAttributes(this, CharacterClass, Level, AbilitySystemComponent);
}


void ALPEnemyCharacter::HighlightActor_Implementation()
{
	bHighlighted = true;
	GetMesh()->SetRenderCustomDepth(true);
	Weapon->SetRenderCustomDepth(true);
}

void ALPEnemyCharacter::UnHighlightActor_Implementation()
{
	bHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

void ALPEnemyCharacter::SetMoveToLocation_Implementation(FVector& OutDestination)
{
	//无操作不更改目的地，攻击范围添加上之后可以修改这里进行移动
}

void ALPEnemyCharacter::SetCombatTarget_Implementation(AActor* InCombatTarget)
{
	CombatTarget = InCombatTarget;
}

AActor* ALPEnemyCharacter::GetCombatTarget_Implementation() const
{
	return CombatTarget;
}

int32 ALPEnemyCharacter::GetPlayerLevel_Implementation()
{
	return Level;
}

void ALPEnemyCharacter::Die(const FVector& DeathImpulse)
{
	SetLifeSpan(LifeSpan);
	if (LPAIController)	LPAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Dead"), true);
	SpawnLoot();
	Super::Die(DeathImpulse);
}
void ALPEnemyCharacter::StunTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	Super::StunTagChanged(CallbackTag, NewCount);
	if (LPAIController && LPAIController->GetBlackboardComponent())
	{
		LPAIController->GetBlackboardComponent()->SetValueAsBool(TEXT("Stunned"), bIsStunned);
	}
}



