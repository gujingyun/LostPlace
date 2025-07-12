// Fill out your copyright notice in the Description page of Project Settings.


#include "Characters/LPPlayerBase.h"

#include "LPGameplayTags.h"
#include "NiagaraComponent.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "AbilitySystem/Data/AbilityInfo.h"
#include "AbilitySystem/Debuff/DebuffNiagaraComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Player/LPPlayerState.h"
#include "Player/LPPlayerController.h"
#include "UI/LPHUD.h"
#include "Characters/CharacterBase.h"
#include "Core/LoadScreenSaveGame.h"
#include "Core/LPGameInstance.h"
#include "Core/LPGameMode.h"
#include "Kismet/GameplayStatics.h"


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

	OnASCRegistered.Broadcast(AbilitySystemComponent);

	//注册监听负面标签变动
	DeBuffRegisterChanged();
	
	//获取PC
	if(ALPPlayerController* PlayerControllerBase = Cast<ALPPlayerController>(GetController()))
	{
		if(ALPHUD* HUD = Cast<ALPHUD>(PlayerControllerBase->GetHUD()))
		{
			HUD->InitOverlay(PlayerControllerBase, PlayerStateBase, AbilitySystemComponent, AttributeSet);
		}
	}

	// InitDefaultAttributes();
}


void ALPPlayerBase::OnRep_Stunned()
{
	if (UAbilitySystemComponentBase* LPASC = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent))
	{
		const FLPGameplayTags& GameplayTags = FLPGameplayTags::Get();
		FGameplayTagContainer BlockedTags;
		BlockedTags.AddTag(GameplayTags.Player_Block_CursorTrace);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputHold);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputPressed);
		BlockedTags.AddTag(GameplayTags.Player_Block_InputReleased);
		if (bIsStunned)
		{
			LPASC->AddLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Activate();
		}
		else
		{
			LPASC->RemoveLooseGameplayTags(BlockedTags);
			StunDebuffComponent->Deactivate();
		}
	}
}

void ALPPlayerBase::OnRep_Burned()
{
	if (bIsBurned)
	{
		BurnDebuffComponent->Activate();
	}
	else
	{
		BurnDebuffComponent->Deactivate();
	}
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
	LoadProgress();
	if (ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this)))
	{
		LPGameMode->LoadWorldState(GetWorld());
	}
}
void ALPPlayerBase::LoadProgress()
{
	ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this));
	if (LPGameMode)
	{
		ULoadScreenSaveGame* SaveData = LPGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;

		//判断是否为第一次加载存档，如果第一次，属性没有相关内容
		if(SaveData->bFirstTimeLoadIn)
		{
			//如果第一次加载存档，使用默认GE初始化主要属性
			InitDefaultAttributes();
			//初始化角色技能
			AddCharacterAbilities();
		}
		else
		{
			if (UAbilitySystemComponentBase* LPASC = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent))
			{
				LPASC->AddCharacterAbilitiesFormSaveData(SaveData);
			}
			if(ALPPlayerState* LPPlayerState = Cast<ALPPlayerState>(GetPlayerState()))
			{
				LPPlayerState->SetLevel(SaveData->PlayerLevel);
				LPPlayerState->SetXP(SaveData->XP);
				LPPlayerState->SetAttributePoints(SaveData->AttributePoints);
				LPPlayerState->SetSpellPoints(SaveData->SpellPoints);
			}
			//如果不是第一次，将通过函数库函数通过存档数据初始化角色属性
			ULPAbilitySystemLibrary::InitializeDefaultAttributesFromSaveData(this, AbilitySystemComponent, SaveData);
		}

	}
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
	if (UAbilitySystemComponentBase* ASC = Cast<UAbilitySystemComponentBase>(GetAbilitySystemComponent()))
	{
		ASC->UpdateAbilityStatuses(PlayerStateBase->GetPlayerLevel());
	}
}

void ALPPlayerBase::AddToAttributePoints_Implementation(int32 InAttributePoints)
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToAttributePoints(InAttributePoints);
}

void ALPPlayerBase::AddToSpellPoints_Implementation(int32 InSpellPoints)
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	PlayerStateBase->AddToSpellPoints(InSpellPoints);
}

int32 ALPPlayerBase::GetAttributePoints_Implementation() const
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->GetAttributePoints();
}

int32 ALPPlayerBase::GetSpellPoints_Implementation() const
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase); //检测是否有效，无限会暂停游戏
	return PlayerStateBase->GetSpellPoints();
}

void ALPPlayerBase::ShowMagicCircle_Implementation(UMaterialInterface* DecalMaterial)
{
	if(ALPPlayerController* PlayerControllerBase = Cast<ALPPlayerController>(GetController()))
	{
		PlayerControllerBase->ShowMagicCircle(DecalMaterial);
		PlayerControllerBase->bShowMouseCursor = false;
	}
}

void ALPPlayerBase::HideMagicCircle_Implementation() const
{
	if(ALPPlayerController* PlayerControllerBase = Cast<ALPPlayerController>(GetController()))
	{
		PlayerControllerBase->HideMagicCircle();
		PlayerControllerBase->bShowMouseCursor = true;
	}
}

void ALPPlayerBase::SaveProgress_Implementation(const FName& CheckpointTag)
{
	ALPGameMode* LPGameMode = Cast<ALPGameMode>(UGameplayStatics::GetGameMode(this));
	if (LPGameMode)
	{
		ULoadScreenSaveGame* SaveData = LPGameMode->RetrieveInGameSaveData();
		if (SaveData == nullptr) return;
		SaveData->PlayerStartTag = CheckpointTag;
		SaveData->ActivatedPlayerStatTags.AddUnique(CheckpointTag);
		//修改玩家相关
		if(const ALPPlayerState* LPPlayerState = Cast<ALPPlayerState>(GetPlayerState()))
		{
			SaveData->PlayerLevel = LPPlayerState->GetPlayerLevel();
			SaveData->XP = LPPlayerState->GetXP();
			SaveData->AttributePoints = LPPlayerState->GetAttributePoints();
			SaveData->SpellPoints = LPPlayerState->GetSpellPoints();
		}

		//修改主要属性
		SaveData->Strength = UAttributeSetBase::GetStrengthAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Intelligence = UAttributeSetBase::GetIntelligenceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Resilience = UAttributeSetBase::GetResilienceAttribute().GetNumericValue(GetAttributeSet());
		SaveData->Vigor = UAttributeSetBase::GetVigorAttribute().GetNumericValue(GetAttributeSet());

		SaveData->bFirstTimeLoadIn = false; //保存完成将第一次加载属性设置为false
		if(!HasAuthority()) return;

		UAbilitySystemComponentBase* LPASC = Cast<UAbilitySystemComponentBase>(AbilitySystemComponent);
		SaveData->SavedAbilities.Empty(); //清空数组

		//使用ASC里创建的ForEach函数循环获取角色的技能，并生成技能结构体保存
		FForEachAbility SaveAbilityDelegate;
		SaveAbilityDelegate.BindLambda([this, LPASC, SaveData](const FGameplayAbilitySpec& AbilitySpec)
		{
			//获取技能标签和
			const FGameplayTag AbilityTag = UAbilitySystemComponentBase::GetAbilityTagFromSpec(AbilitySpec);
			UAbilityInfo* AbilityInfo = ULPAbilitySystemLibrary::GetAbilityInfo(this);
			FLPAbilityInfo Info = AbilityInfo->FindAbilityInfoForTag(AbilityTag);

			//创建技能结构体
			FSavedAbility SavedAbility;
			SavedAbility.GameplayAbility = Info.Ability;
			SavedAbility.AbilityLevel = AbilitySpec.Level;
			SavedAbility.AbilityTag = AbilityTag;
			SavedAbility.AbilitySlot = LPASC->GetSlotFromAbilityTag(AbilityTag);
			SavedAbility.AbilityStatus = LPASC->GetStatusFromAbilityTag(AbilityTag);
			// SavedAbility.AbilityInputTag = LPASC->GetInputTagFromAbilityTag(AbilityTag);
			SavedAbility.AbilityType = Info.AbilityType;
			
			SaveData->SavedAbilities.AddUnique(SavedAbility);
		});
		//调用ForEach技能来执行存储到存档
		LPASC->ForEachAbility(SaveAbilityDelegate);
		
		LPGameMode->SaveInGameProgressData(SaveData);
	}
}

int32 ALPPlayerBase::GetPlayerLevel_Implementation()
{
	ALPPlayerState* PlayerStateBase = GetPlayerState<ALPPlayerState>();
	check(PlayerStateBase);
	return PlayerStateBase->GetPlayerLevel();
}
