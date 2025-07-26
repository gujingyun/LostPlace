// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InputActionValue.h"
#include "InputMappingContext.h"
#include "GameplayTagContainer.h"
#include "Components/SplineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Interface/EnemyInterface.h"
#include "Player/Inv_PlayerController.h"
#include "Player/Inv_PlayerController.h"
#include "LPPlayerController.generated.h"
class IHighlightInterface;
class AMagicCircle;
class UNiagaraSystem;
class UInputConfig;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;
class UAbilitySystemComponentBase;
class UDamageTextComponent;
//鼠标拾取目标的状态枚举
enum class ETargetingStatus : uint8
{
	//敌人
	TargetingEnemy,
	//鼠标拾取的目标非敌人
	TargetingNonEnemy,
	//无
	NotTargeting
};

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ALPPlayerController : public AInv_PlayerController
{
	GENERATED_BODY()
public:
	ALPPlayerController();
	virtual void PlayerTick(float DeltaTime) override;

	UFUNCTION(Client, Reliable)
	void ShowDamageNumber(float DamageAmount, ACharacter* TargetCharacter,bool bBlockedHit,bool bCriticalHit); //在每个客户端显示伤害数值

	//显示魔法光圈 并设置光圈贴花材质
	UFUNCTION(BlueprintCallable)
	void ShowMagicCircle(UMaterialInterface* DecalMaterial);

	//隐藏魔法光圈
	UFUNCTION(BlueprintCallable)
	void HideMagicCircle() const;




protected:
	virtual void BeginPlay() override;
	// Called to bind functionality to input
	virtual void SetupInputComponent() override;
private:
	
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> CharacterContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> MoveAction;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> ShiftAction;

	void ShiftPressed(){bShiftKeyDown = true;};
	void ShiftReleased(){bShiftKeyDown = false;};
	bool bShiftKeyDown = false;
	
	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> LookAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	
	TObjectPtr<UInputAction> AttackAction;
	
	/** Called for movement input */
	void Move(const FInputActionValue& Value);
	
	void AttackTriggered(const FInputActionValue& Value);

	void CursorTrace();//鼠标追踪

	// TObjectPtr<AActor> LastActor;
	// TObjectPtr<AActor> ThisActor;
	
	TWeakObjectPtr<AActor> ThisActor;
	TWeakObjectPtr<AActor> LastActor;
	
	FHitResult CursorHit;
	static  void HighlightActor(AActor* InActor);
	static void UnHighlightActor(AActor* InActor);
	
	void AbilityInputTagPressed(FGameplayTag InputTag);
	void AbilityInputTagReleased(FGameplayTag InputTag);
	void AbilityInputTagHold(FGameplayTag InputTag);

	
	UPROPERTY(EditDefaultsOnly, Category="Input")
	TObjectPtr<UInputConfig> InputConfig;

	UPROPERTY()
	TObjectPtr<UAbilitySystemComponentBase> AbilitySystemComponentBase;
	
	UAbilitySystemComponentBase* GetASC();



	FVector CachedDestination = FVector::ZeroVector; //存储鼠标点击的位置
	float FollowTime = 0.f; // 用于查看按住了多久
	bool bAutoRunning = false; //当前是否自动移动
	// bool bTargeting = false; //当前鼠标是否选中敌人
	
	ETargetingStatus TargetingStatus = ETargetingStatus::NotTargeting;
	
	UPROPERTY(EditDefaultsOnly)
	float ShortPressThreshold = 0.3f; //定义鼠标悬停多长时间内算点击事件

	UPROPERTY(EditDefaultsOnly)
	float AutoRunAcceptanceRadius = 50.f; //当角色和目标距离在此半径内时，将关闭自动寻路

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USplineComponent> Spline; //自动寻路时生成的样条线

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> ClickNiagaraSystem;
	
	void AutoRun();

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UDamageTextComponent> DamageTextComponentClass;

	//创建奥数光圈使用的类
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AMagicCircle> MagicCircleClass;

	//存储魔法光圈的属性，不需要暴露给蓝图
	UPROPERTY()
	TObjectPtr<AMagicCircle> MagicCircle;

	void UpdateMagicCircleLocation() const; //每一帧调用，更新魔法光圈的位置

};
