#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Enemy.h"
#include "Engine/TimerHandle.h"
#include "FPSSurvivorGameModeBase.h"
#include "TopDownCharacter.h"
#include "EnemySpawner.generated.h"

UCLASS()
class FPSSURVIVOR_API AEnemySpawner : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AEnemy> EnemyActorToSpawn;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTime = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnDistance = 450.f;

	ATopDownCharacter* Player;

	// Keep track total enemys spawn
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	int32 TotalEnemyCount = 0;

	// Interval when we increase difficulity 
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 DifficultySpikeInterval = 15;

	// Spawntime will not go below 0.5
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float SpawnTimeMinimumLimit = 0.5f;

	// When we are decreasing the spawn time
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float DecreaseSpawnTimerByEveryInterval = 0.05f;

	FTimerHandle SpawnTimer;

	AFPSSurvivorGameModeBase* MyGameMode;

	virtual void Tick(float DeltaTime) override;

	AEnemySpawner();

	virtual void BeginPlay() override;

	void OnSpawnTimerTimeout();
	void StartSpawning();
	void StopSpawning();
	void SpawnEnemy();
	void SetupEnemy(AEnemy* Enemy);

	// Use this macro to bind delegate, arguments must be the same as declared
	UFUNCTION()
	void OnEnemyDied();

	UFUNCTION()
	void OnPlayerDied();
};
