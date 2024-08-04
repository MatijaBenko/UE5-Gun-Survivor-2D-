// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/InputComponent.h"
#include "InputActionValue.h"
#include "GameFramework/Controller.h"
#include "Bullet.h"
#include "Components/SceneComponent.h"
#include "PaperSpriteComponent.h"
#include "Engine/TimerHandle.h"
#include "TopDownCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPlayerDiedDelegate);
UCLASS()
class FPSSURVIVOR_API ATopDownCharacter : public APawn
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent* CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent* CharacterFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* IdleFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperFlipbook* RunFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* GunParent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UPaperSpriteComponent* GunSprite;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	USceneComponent* BulletSpawnPosition;

	//X Left, Y Right limits
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D HorizontalLimits;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector2D VerticalLimits;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ABullet> BulletActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputMappingContext* InputMappingContext;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UInputAction* FireAction;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 100.f;

	UPROPERTY(BlueprintReadWrite)
	FVector2D MovementDirection;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanMove = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanShoot = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	float ShootCoolDownDurationInSeconds = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* DeathSound;

	FTimerHandle ShootCoolDownTimer;

	FPlayerDiedDelegate PlayerDiedDelegate;

	ATopDownCharacter();

	virtual void Tick(float DeltaTime) override;

	virtual void BeginPlay() override;

	virtual void SetupPlayerInputComponent(class UInputComponent *PlayerInputComponent) override;

	// Trigger Input Action Events that Unreal will call, this is the Syntax the engine wants
	// We then bind these to our Input Mapping Context
	void MoveTriggered(const FInputActionValue& Value);
	void MoveCompleted(const FInputActionValue& Value);

	void Fire(const FInputActionValue& Value);

	bool IsInMapBoundsHorizontal(float XPos);

	bool IsInMapBoundsVertical(float ZPos);

	void OnShootCoolDownTimerTimeout();

	UFUNCTION()
	void OverlapBegin(UPrimitiveComponent *OverLappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
					  int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
};
