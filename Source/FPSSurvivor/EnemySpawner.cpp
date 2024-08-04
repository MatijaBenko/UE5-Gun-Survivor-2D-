#include "Kismet/GameplayStatics.h"
#include "EnemySpawner.h"

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();
	AGameModeBase* GameMode = UGameplayStatics::GetGameMode(GetWorld());
	if (GameMode)
	{
		MyGameMode = Cast<AFPSSurvivorGameModeBase>(GameMode);
	}
	// Get actors of all in the game, if it matches the type takes it and give us the reference
	AActor *PlayerActor = UGameplayStatics::GetActorOfClass(GetWorld(), ATopDownCharacter::StaticClass());
	if (PlayerActor)
	{
		// Cast is needed as Kismets return a AActor pointer, but we are confident its the type we are interested in
		Player = Cast<ATopDownCharacter>(PlayerActor);
		Player->PlayerDiedDelegate.AddDynamic(this, &AEnemySpawner::OnPlayerDied);
	}

	StartSpawning();
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AEnemySpawner::OnSpawnTimerTimeout()
{
	SpawnEnemy();
}
void AEnemySpawner::StartSpawning()
{
	// Setting True make this continue to have a timer to spawn enemy
	GetWorldTimerManager().SetTimer(SpawnTimer, this, &AEnemySpawner::OnSpawnTimerTimeout, SpawnTime, true, SpawnTime);
}
void AEnemySpawner::StopSpawning()
{
	GetWorldTimerManager().ClearTimer(SpawnTimer);
}
void AEnemySpawner::SpawnEnemy()
{
	// Get a random value in the circle normalize to 1 unit then multiple by distance as we increase the radius
	FVector2D RandomPosition = FVector2D(FMath::VRand());
	RandomPosition.Normalize();
	// Spawn distance represents radius of circle
	RandomPosition *= SpawnDistance;

	// GetActorLocation will spawn based where we placed the spawner
	FVector EnemyLocation = GetActorLocation() + FVector(RandomPosition.X, 0.f, RandomPosition.Y);

	AEnemy *Enemy = GetWorld()->SpawnActor<AEnemy>(EnemyActorToSpawn, EnemyLocation, FRotator::ZeroRotator);
	SetupEnemy(Enemy);

	// Difficult System Logic
	TotalEnemyCount += 1;
	if (TotalEnemyCount % DifficultySpikeInterval == 0)
	{
		if (SpawnTime > SpawnTimeMinimumLimit)
		{
			SpawnTime -= DecreaseSpawnTimerByEveryInterval;
			if (SpawnTime < SpawnTimeMinimumLimit)
			{
				SpawnTime = SpawnTimeMinimumLimit;
			}
			// Update time as it will use the old SpawnTime
			StopSpawning();
			StartSpawning();
		}
	}
}

void AEnemySpawner::SetupEnemy(AEnemy* Enemy)
{
	if (Enemy) 
	{
		Enemy->Player = Player;
		Enemy->CanFollow = true;
		Enemy->MovementSpeed = FMath::FRandRange(Enemy->MovementSpeedMin, Enemy->MovementSpeedMax);
		Enemy->EnemyDiedDelegate.AddDynamic(this, &AEnemySpawner::OnEnemyDied);
	}
}

void AEnemySpawner::OnEnemyDied()
{
	int ScoreToAdd = FMath::RandRange(10, 25);
	MyGameMode->AddScore(ScoreToAdd);
}

void AEnemySpawner::OnPlayerDied()
{
	StopSpawning();

	TArray<AActor*> EnemyArray;
	// Get all actor of class
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AEnemy::StaticClass(), EnemyArray);
	for (AActor* EnemyActor : EnemyArray)
	{
		AEnemy* Enemy = Cast<AEnemy>(EnemyActor);
		if (Enemy && Enemy->IsAlive)
		{
			Enemy->CanFollow = false;
		}
	}

	MyGameMode->RestartGame();
}