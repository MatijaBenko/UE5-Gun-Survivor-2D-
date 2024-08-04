#include "Enemy.h"
#include "Kismet/GameplayStatics.h"

AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	EnemyFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("EnemyFlipbookComp"));
	EnemyFlipbook->SetupAttachment(RootComponent);
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
}

void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsAlive && CanFollow && Player)
	{
		// Move towards the player
		FVector CurrentLocation = GetActorLocation();
		FVector PlayerLocation = Player->GetActorLocation();
		FVector DirectionToPlayer = PlayerLocation - CurrentLocation;
		float DistanceToPlayer = DirectionToPlayer.Length();

		if (DistanceToPlayer > StopDistance)
		{
			// If we normalize before getting length would be 1
			DirectionToPlayer.Normalize();

			FVector NewLocation = CurrentLocation + (DirectionToPlayer * MovementSpeed * DeltaTime);
			SetActorLocation(NewLocation);
		}

		// Face the Player
		CurrentLocation = GetActorLocation();
		float FlipbookXScale = EnemyFlipbook->GetComponentScale().X;

		// Last case is if the enemy and player are on the same x axis value, we cover this and have them look to the right
		// Hence > or =
		// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, FString::SanitizeFloat(FlipbookXScale));
		if (PlayerLocation.X - CurrentLocation.X >= 0.f) // Player is on the right side of the enemy
		{

			if (FlipbookXScale < 0.f)
			{
				EnemyFlipbook->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
			}
		}
		else // Player on the left side of the enemy
		{
			if (FlipbookXScale > 0.f)
			{
				EnemyFlipbook->SetWorldScale3D(FVector(-1.f, 1.f, 1.f));
			}
		}
	}
}

void AEnemy::Die()
{
	if (!IsAlive)
		return;

	if (DeathSound)
	{
		UGameplayStatics::PlaySound2D(this, DeathSound);
	}
	
	IsAlive = false;
	CanFollow = false;

	// We just need to update the asset not need another flipbook
	EnemyFlipbook->SetFlipbook(DeadFlipbookAsset);
	EnemyFlipbook->SetTranslucentSortPriority(-2);

	// Fire the delegate
	EnemyDiedDelegate.Broadcast();

	float DestoryTime = 5.f;
	GetWorldTimerManager().SetTimer(DestoryTimer, this, &AEnemy::OnDestoryTimerTimeout, 1.0f, false, DestoryTime);
}

void AEnemy::OnDestoryTimerTimeout()
{
	Destroy();
}