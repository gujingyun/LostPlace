// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/LPPlayerController.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "EnhancedInputSubsystems.h"
#include "LPGameplayTags.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "NiagaraFunctionLibrary.h"
#include "AbilitySystem/AbilitySystemComponentBase.h"
#include "AbilitySystem/LPAbilitySystemLibrary.h"
#include "Actors/EffectActorBase.h"
#include "Actors/MagicCircle.h"
#include "Components/DecalComponent.h"
#include "Input/InputComponentBase.h"
#include "GameFramework/Character.h"
#include "Interface/HighlightInterface.h"
#include "LostPlace/LostPlace.h"
#include "UI/DamageTextComponent.h"
// Sets default values
ALPPlayerController::ALPPlayerController()
{
	bReplicates = true;
	LastActor = nullptr;
	ThisActor = nullptr;

	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}


void ALPPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FLPGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	if(InputTag.MatchesTagExact(FLPGameplayTags::Get().InputTag_LMB))
	{
		
		// if (IsValid(ThisActor))
		if (ThisActor.IsValid())
		{
			TargetingStatus = ThisActor->Implements<UEnemyInterface>() ? ETargetingStatus::TargetingEnemy : ETargetingStatus::TargetingNonEnemy;
		}else
		{
			TargetingStatus = ETargetingStatus::NotTargeting;
		}
		bAutoRunning = false;
		FollowTime = 0.f; //重置统计的时间
	}
	if (GetASC()) GetASC()->AbilityInputTagPressed(InputTag);
}

void ALPPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FLPGameplayTags::Get().Player_Block_InputReleased))
	{
		return;
	}
	if (!InputTag.MatchesTagExact(FLPGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagReleased(InputTag);
		}
		return;
	}
	if (GetASC())
	{
		GetASC()->AbilityInputTagReleased(InputTag);
	}
	// if (ThisActor.IsValid())
	if (ThisActor.IsValid() && ThisActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_SetMoveToLocation(ThisActor.Get(), CachedDestination);
	}
	else if (GetASC() && !GetASC()->HasMatchingGameplayTag(FLPGameplayTags::Get().Player_Block_InputPressed))
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(this, ClickNiagaraSystem, CachedDestination);
	}
	
	if (TargetingStatus!=ETargetingStatus::TargetingEnemy && !bShiftKeyDown)
	{
		const APawn* ControlledPawn = GetPawn();
		if(FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			if(UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				Spline->ClearSplinePoints(); //清除样条内现有的点
				for(const FVector& PointLoc : NavPath->PathPoints)
				{
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World); //将新的位置添加到样条曲线中
					// DrawDebugSphere(GetWorld(), PointLoc, 8.f, 8, FColor::Orange, false, 5.f); //点击后debug调试
				}
				if (NavPath->PathPoints.Num() > 0)
				{
					//自动寻路将最终目的地设置为导航的终点，方便停止导航
					CachedDestination = NavPath->PathPoints[NavPath->PathPoints.Num() - 1];
					bAutoRunning = true; //设置当前正常自动寻路状态，将在tick中更新位置
				}
			}
			
		}
		TargetingStatus = ETargetingStatus::NotTargeting;
		FollowTime = 0.f; //重置统计的时间
	}
}

void ALPPlayerController::AbilityInputTagHold(FGameplayTag InputTag)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FLPGameplayTags::Get().Player_Block_InputHold))
	{
		return;
	}
	if (!InputTag.MatchesTagExact(FLPGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
		return;
	}
	if (TargetingStatus==ETargetingStatus::TargetingEnemy || bShiftKeyDown)
	{
		if (GetASC())
		{
			GetASC()->AbilityInputTagHeld(InputTag);
		}
	}else
	{
		FollowTime += GetWorld()->GetDeltaSeconds(); //统计悬停时间来判断是否为点击
		
		if(CursorHit.bBlockingHit)
		{
			CachedDestination = CursorHit.ImpactPoint;
		}

		if(APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
	
}

UAbilitySystemComponentBase* ALPPlayerController::GetASC()
{
	if (AbilitySystemComponentBase == nullptr)
	{
		AbilitySystemComponentBase = Cast<UAbilitySystemComponentBase>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return AbilitySystemComponentBase;
}

// Called when the game starts or when spawned
void ALPPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(CharacterContext); //判断是否存在

	//从本地角色身上获取到它的子系统
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());//多人游戏中，每个玩家的客户端都有一个本地玩家和多个非本地玩家，假如有三个玩家，除了本地玩家，其他玩家的子系统都是空的
	if(Subsystem)//检查子系统是否存在
	{
		Subsystem->AddMappingContext(CharacterContext, 1); //可以存在多个操作映射，根据优先级触发
	} 
	

	bShowMouseCursor = true; //游戏中是否显示鼠标光标
	DefaultMouseCursor = EMouseCursor::Default; //鼠标光标的样式

	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); //将鼠标锁定在视口内
	InputModeData.SetHideCursorDuringCapture(false); //鼠标被捕获时是否隐藏
	SetInputMode(InputModeData); //设置给控制器

}
//////////////////////////////////////////////////////////////////////////
// Input

void ALPPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	// Set up action bindings
	if (UInputComponentBase* EnhancedInputComponent  = Cast<UInputComponentBase>(InputComponent)) {
		// Moving
		EnhancedInputComponent ->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ALPPlayerController::Move);

		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ALPPlayerController::ShiftPressed);
		EnhancedInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ALPPlayerController::ShiftReleased);
		
		EnhancedInputComponent ->BindAbilityAction(InputConfig,this,&ThisClass::AbilityInputTagPressed,&ThisClass::AbilityInputTagReleased,&ThisClass::AbilityInputTagHold);
		
		// EnhancedInputComponent->BindAction(PrimaryInteractAction, ETriggerEvent::Started, this, &ThisClass::PrimaryInteract);
		// EnhancedInputComponent->BindAction(ToggleInventoryAction, ETriggerEvent::Started, this, &ThisClass::ToggleInventory);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void ALPPlayerController::Move(const FInputActionValue& Value)
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FLPGameplayTags::Get().Player_Block_InputPressed))
	{
		return;
	}
	Spline->ClearSplinePoints();
	const FVector2D InputAxisVector = Value.Get<FVector2D>(); //获取输入操作的2维向量值
	const FRotator Rotation = GetControlRotation(); //获取控制器旋转
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f); //通过控制器的垂直朝向创建一个旋转值，忽略上下朝向和左右朝向

	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X); //获取世界坐标系下向前的值，-1到1
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y); //获取世界坐标系下向右的值，-1到1
	// const FVector ForwardDirection = FVector::ForwardVector; // 即 X轴，W/S
	// const FVector RightDirection   = FVector::RightVector;   // 即 Y轴，A/D
	if(APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}

void ALPPlayerController::AttackTriggered(const FInputActionValue& Value)
{
	UE_LOG(LogTemp,Log,TEXT("点击事件"));
}

//帧更新
void ALPPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);

	CursorTrace();
	AutoRun();
	UpdateMagicCircleLocation();
}

void ALPPlayerController::ShowMagicCircle(UMaterialInterface* DecalMaterial)
{
	if(!IsValid(MagicCircle)) MagicCircle = GetWorld()->SpawnActor<AMagicCircle>(MagicCircleClass);

	//设置贴花材质
	if(DecalMaterial != nullptr)
	{
		MagicCircle->MagicCircleDecal->SetMaterial(0, DecalMaterial);
	}
}

void ALPPlayerController::HideMagicCircle() const
{
	if(IsValid(MagicCircle)) MagicCircle->Destroy();
}



void ALPPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter,bool bBlockedHit,bool bCriticalHit)
{
	//确保传入的目标没被销毁并且设置了组件类
	
	if(IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent(); //动态创建的组件需要调用注册
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform); //先附加到角色身上，使用角色位置
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform); //然后从角色身上分离，保证在一个位置播放完成动画
		DamageText->SetDamageText(DamageAmount,bBlockedHit,bCriticalHit); //设置显示的伤害数字
	}
}

void ALPPlayerController::AutoRun()
{
	if(!bAutoRunning) return;
	if(APawn* ControlledPawn = GetPawn())
	{
		//找到距离样条最近的位置
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		//获取这个位置在样条上的方向
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		ControlledPawn->AddMovementInput(Direction);

		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		if(DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			bAutoRunning = false;
		}
	}
}

void ALPPlayerController::UpdateMagicCircleLocation() const
{
	if(IsValid(MagicCircle))
	{
		if(IsValid(MagicCircle))
		{
			MagicCircle->SetActorLocation(CursorHit.ImpactPoint);
		}
	}
}
void ALPPlayerController::HighlightActor(AActor* InActor)
{
	if (IsValid(InActor)&&InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_HighlightActor(InActor);
	}
}

void ALPPlayerController::UnHighlightActor(AActor* InActor)
{
	if (IsValid(InActor)&&InActor->Implements<UHighlightInterface>())
	{
		IHighlightInterface::Execute_UnHighlightActor(InActor);
	}
}

//鼠标位置追踪
void ALPPlayerController::CursorTrace()
{
	if (GetASC() && GetASC()->HasMatchingGameplayTag(FLPGameplayTags::Get().Player_Block_CursorTrace))
	{

		UnHighlightActor(LastActor.Get());
		UnHighlightActor(ThisActor.Get());
		LastActor = nullptr;
		ThisActor = nullptr;
		return;
	}
	const ECollisionChannel TraceChannel = IsValid(MagicCircle) ? ECC_ExcludePlayers : ECC_Visibility;
	GetHitResultUnderCursor(TraceChannel, false, CursorHit); //获取可视的鼠标命中结果
	if(!CursorHit.bBlockingHit) return; //如果未命中直接返回

	LastActor = ThisActor;
	if (IsValid(CursorHit.GetActor())&&CursorHit.GetActor()->Implements<UHighlightInterface>())
	{
		ThisActor = CursorHit.GetActor();
	}else
	{
		ThisActor = nullptr;
	}
	
	/**
	 * 射线拾取后，会出现的几种情况
	 * 1. LastActor is null   ThisActor is null 不需要任何操作
	 * 2. LastActor is null   ThisActor is valid 高亮ThisActor
	 * 3. LastActor is valid   ThisActor is null 取消高亮LastActor
	 * 4. LastActor is valid   ThisActor is valid LastActor ！= ThisActor 取消高亮LastActor 高亮ThisActor
	 * 5. LastActor is valid   ThisActor is valid LastActor == ThisActor 不需要任何操作
	 */

	if (LastActor.Get() != ThisActor.Get())
	{
		UnHighlightActor(LastActor.Get());
		HighlightActor(ThisActor.Get());
	}
	// if(LastActor == nullptr)
	// {
	// 	if(ThisActor != nullptr)
	// 	{
	// 		//case 2
	// 		ThisActor->HighlightActor();
	// 	} // else case 1
	// }
	// else
	// {
	// 	if(ThisActor == nullptr)
	// 	{
	// 		//case 3
	// 		LastActor->UnHighlightActor();
	// 	}
	// 	else
	// 	{
	// 		if(LastActor != ThisActor)
	// 		{
	// 			//case 4
	// 			LastActor->UnHighlightActor();
	// 			ThisActor->HighlightActor();
	// 		} //else case 5
	// 	}
	// }
	
}
