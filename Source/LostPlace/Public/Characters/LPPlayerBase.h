// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterBase.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "Player/LPPlayerState.h"
#include "Player/LPPlayerController.h"
#include "UI/MyHUD.h"
#include "LPPlayerBase.generated.h"
class USpringArmComponent;
class UCameraComponent;

/**
 * 
 */
UCLASS()
class LOSTPLACE_API ALPPlayerBase : public ACharacterBase
{
	GENERATED_BODY()
public:
	// Sets default values for this character's properties
	ALPPlayerBase();

	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Returns CameraBoom subobject **/
	FORCEINLINE  USpringArmComponent* GetCameraBoom() const { return CameraBoom; }
	/** Returns FollowCamera subobject **/
	FORCEINLINE  UCameraComponent* GetFollowCamera() const { return FollowCamera; }

	virtual void PossessedBy(AController* NewController) override;//服务器初始化ASC
	
	virtual void OnRep_PlayerState() override;//客户端初始化ASC

	/** 实现ICombatInterface接口 */
	virtual int32 GetPlayerLevel() override; //获取等级
	/** 实现ICombatInterface接口结束 */
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* CameraBoom;
	
	/** Follow camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FollowCamera;

	
private:
	virtual void InitAbilityActorInfo() override;
};
