#include "Bullet.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"

ABullet::ABullet()
{
	PrimaryActorTick.bCanEverTick = true;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SetRootComponent(SphereComp);

	BulletSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BulletSprite"));
	BulletSprite->SetupAttachment(RootComponent);

	// Default dirction
	MovementDirection = FVector2D(1.f, 0.f);
}

void ABullet::BeginPlay()
{
	Super::BeginPlay();

	// Binds the function UE5 will call to our event when actor collision is overlapped
	SphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABullet::OverlapBegin);
}

void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (IsLaunched)
	{
		// Handles the logic for the tragectory of the bullet
		FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;
		FVector CurrLocation = GetActorLocation();
		FVector NewLocation = CurrLocation + FVector(DistanceToMove.X, 0.f, DistanceToMove.Y);
		SetActorLocation(NewLocation);
		//
	}
}

// Input would be provided by the player 
void ABullet::Launch(FVector2D Direction, float Speed)
{
	if (IsLaunched) return;

	if (BulletFireSound)
	{
		UGameplayStatics::PlaySound2D(this, BulletFireSound);
	}

	IsLaunched = true;

	MovementDirection = Direction;
	MovementSpeed = Speed;

	float DeleteTime = 10.f;
	GetWorldTimerManager().SetTimer(fTimer,this, &ABullet::OnDeleteTimerTimeout, 1.f, false, DeleteTime);
}

void ABullet::OnDeleteTimerTimeout()
{
	Destroy();
}

void ABullet::OverlapBegin(UPrimitiveComponent *OverLappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
									 int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AEnemy* EnemyActor = Cast<AEnemy>(OtherActor);
	if (EnemyActor && EnemyActor->IsAlive)
	{
		DisableBullet();
		EnemyActor->Die();
	}
}

void ABullet::DisableBullet()
{
	if (IsDisable) return;

	IsDisable = true;

	// Stop comp from colliding
	SphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	// Destory sprite so its invisible no need to delete actor as we do this in our timer timeout
	BulletSprite->DestroyComponent();
}