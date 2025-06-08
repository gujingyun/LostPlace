// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LPPlayerBase.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LPPlayerState.h"
#include "Player/LPPlayerController.h"
#include "UI/LPHUD.h"
#include "Characters/CharacterBase.h"


ALPPlayerBase::ALPPlayerBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	GetCharacterMovement()->bOrientRotationToMovement = true; //设置为true，角色将朝移动的方向旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f); //旋转速率
	GetCharacterMovement()->bConstrainToPlane = true; //约束到平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true; //设置了上面一项为true，且此项设置为true，则在开始时与地面对其

	CharacterClass = ECharacterClass::Elementalist;
	// 创建 SpringArm（弹簧臂）
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritYaw = false;
	CameraBoom->bInheritRoll = false;
	
	CameraBoom->SetupAttachment(RootComponent); // 附加到角色根组件
	CameraBoom->TargetArmLength = 800.f; // 镜头拉远距离
	CameraBoom->bUsePawnControlRotation = false; // 不使用控制器旋转
	CameraBoom->SetRelativeRotation(FRotator(-60.f, 0.f, 0.f)); // 俯视角
	CameraBoom->bDoCollisionTest = false;
	// 创建 Camera（相机）
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // 附加到弹簧臂末端
	FollowCamera->bUsePawnControlRotation = false; // 相机不随 Pawn 旋转

	//设置升级特效组件
	LevelUpNiagaraComponent = CreateDefaultSubobject<UNiagaraComponent>(TEXT("LevelUpNiagaraComponent"));
	LevelUpNiagaraComponent->SetupAttachment(GetRootComponent()); //设置附加组件
	LevelUpNiagaraComponent->bAutoActivate = false; //设置不自动激活


	
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponentBase>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true); //设置组件用于在网络上复制
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UAttributeSetBase>("AttributeSet");


}
void ALPPlayerBase::InitAbilityActorInfo()
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase);

	//从playerState获取ASC和AS
	AbilitySystemComponent = PlayerStateBase->GetAbilitySystemComponent();
	AttributeSet = PlayerStateBase->GetAttributeSet();
	//初始化ASC
	AbilitySystemComponent->InitAbilityActorInfo(PlayerStateBase, this);
	
	Cast<UAbilitySystemComponentBase>(PlayerStateBase->GetAbilitySystemComponent())->AbilityActorInfoSet();
	
	//获取PC
	if(ALPPlayerController* PlayerControllerBase = Cast<ALPPlayerController>(GetController()))
	{
		if(ALPHUD* HUD = Cast<ALPHUD>(PlayerControllerBase->GetHUD()))
		{
			HUD->InitOverlay(PlayerControllerBase, PlayerStateBase, AbilitySystemComponent, AttributeSet);
		}
	}

	InitDefaultAttributes();
}



// Called when the game starts or when spawned
void ALPPlayerBase::BeginPlay()
{
	Super::BeginPlay();
	
}
void ALPPlayerBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}



void ALPPlayerBase::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	//初始化ASC的OwnerActor和AvatarActor
	InitAbilityActorInfo();
	
	SetOwner(NewController);
	
	AddCharacterAbilities();
}

void ALPPlayerBase::OnRep_PlayerState()
{
	Super::OnRep_PlayerState();

	//初始化ASC的OwnerActor和AvatarActor
	InitAbilityActorInfo();
}

void ALPPlayerBase::AddToXP_Implementation(int32 InXP)
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游
	PlayerStateBase->AddToXP(InXP);
}

void ALPPlayerBase::LevelUp_Implementation()
{
	MulticastLevelUpParticles();
}
void ALPPlayerBase::MulticastLevelUpParticles_Implementation() const
{
	if(IsValid(LevelUpNiagaraComponent))
	{
		const FVector CameraLocation = FollowCamera->GetComponentLocation();
		const FVector NiagaraSystemLocation = LevelUpNiagaraComponent->GetComponentLocation();
		const FRotator TopCameraRotation = (CameraLocation - NiagaraSystemLocation).Rotation(); //获取相机位置和升级特效的朝向
		LevelUpNiagaraComponent->SetWorldRotation(TopCameraRotation); //设置粒子的转向
		LevelUpNiagaraComponent->Activate(true); //激活特效
	}
}
int32 ALPPlayerBase::GetXP_Implementation() const
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase);
	return PlayerStateBase->GetXP();
}

int32 ALPPlayerBase::FindLevelForXP_Implementation(int32 InXP) const
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase);
	return PlayerStateBase->LevelUpInfo->FindLevelForXP(InXP);
}

int32 ALPPlayerBase::GetAttributePointsReward_Implementation(int32 Level) const
{
	const ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->LevelUpInfo->LevelUpInformation[Level].AttributePointAward;
}

int32 ALPPlayerBase::GetSpellPointsReward_Implementation(int32 Level) const
{
	const ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->LevelUpInfo->LevelUpInformation[Level].SpellPointAward;
}

void ALPPlayerBase::AddToPlayerLevel_Implementation(int32 InPlayerLevel)
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToLevel(InPlayerLevel);
}

void ALPPlayerBase::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	//TODO: 增加属性点
}

void ALPPlayerBase::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	//TODO: 增加技能点
	
}

int32 ALPPlayerBase::GetPlayerLevel_Implementation()
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase);
	return PlayerStateBase->GetPlayerLevel();
}
