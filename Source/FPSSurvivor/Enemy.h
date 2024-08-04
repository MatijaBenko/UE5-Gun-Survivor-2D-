#pragma once

#include "CoreMinimal.h"
#include "Components/CapsuleComponent.h"
#include "PaperFlipbookComponent.h"
#include "GameFramework/Actor.h"
#include "TopDownCharacter.h"
#include "Engine/TimerHandle.h"
#include "Enemy.generated.h"

// This delegate creates a new macro for us
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FEnemyDiedDelegate);
UCLASS()
class FPSSURVIVOR_API AEnemy : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UCapsuleComponent *CapsuleComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	UPaperFlipbookComponent *EnemyFlipbook;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPaperFlipbook *DeadFlipbookAsset;

	// have this to know where the enemy should go
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	ATopDownCharacter *Player;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool IsAlive = true;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool CanFollow = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeedMin = 15.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeedMax = 85.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MovementSpeed = 50.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float StopDistance = 20.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase *DeathSound;
	
	FTimerHandle DestoryTimer;

	FEnemyDiedDelegate EnemyDiedDelegate;

	AEnemy();

	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	void Die();

	void OnDestoryTimerTimeout();
};
