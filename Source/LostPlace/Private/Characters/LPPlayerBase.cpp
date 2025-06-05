// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LPPlayerBase.h"

#include "AbilitySystem/AbilitySystemComponentBase.h"


ALPPlayerBase::ALPPlayerBase()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	GetCharacterMovement()->bOrientRotationToMovement = true; //设置为true，角色将朝移动的方向旋转
	GetCharacterMovement()->RotationRate = FRotator(0.f, 400.f, 0.f); //旋转速率
	GetCharacterMovement()->bConstrainToPlane = true; //约束到平面
	GetCharacterMovement()->bSnapToPlaneAtStart = true; //设置了上面一项为true，且此项设置为true，则在开始时与地面对其

	
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
		if(AMyHUD* HUD = Cast<AMyHUD>(PlayerControllerBase->GetHUD()))
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

int32 ALPPlayerBase::GetPlayerLevel()
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase);
	return PlayerStateBase->GetPlayerLevel();
}
