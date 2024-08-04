// Fill out your copyright notice in the Description page of Project Settings.

#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Enemy.h"
#include "TopDownCharacter.h"

// Sets default values
ATopDownCharacter::ATopDownCharacter()
{
	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(CapsuleComp);

	CharacterFlipbook = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("FlipbookComp"));
	CharacterFlipbook->SetupAttachment(RootComponent);

	GunParent = CreateDefaultSubobject<USceneComponent>(TEXT("GunParentSceneComp"));
	GunParent->SetupAttachment(RootComponent);

	GunSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GunSpriteComp"));
	GunSprite->SetupAttachment(GunParent);

	BulletSpawnPosition = CreateDefaultSubobject<USceneComponent>(TEXT("GunBulletSpawnSceneComp"));
	BulletSpawnPosition->SetupAttachment(GunSprite);
}

// Called when the game starts or when spawned
void ATopDownCharacter::BeginPlay()
{
	Super::BeginPlay();

	// Default we have access to the Controller
	APlayerController *PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		// Always show mouse cursor
		PlayerController->SetShowMouseCursor(true);
		// Get the subsystem but needs a playercontroller which is why we obtain it
		UEnhancedInputLocalPlayerSubsystem *SubSystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer());
		if (SubSystem)
		{
			// This is needed to register our mapping context
			SubSystem->AddMappingContext(InputMappingContext, 0);
		}
	}
	CapsuleComp->OnComponentBeginOverlap.AddDynamic(this, &ATopDownCharacter::OverlapBegin);
}

bool ATopDownCharacter::IsInMapBoundsHorizontal(float XPos)
{
	bool Result = true;

	Result = (XPos > HorizontalLimits.X && XPos < HorizontalLimits.Y);

	return Result;
}

bool ATopDownCharacter::IsInMapBoundsVertical(float ZPos)
{
	bool Result = true;

	Result = (ZPos > VerticalLimits.X && ZPos < VerticalLimits.Y);

	return Result;
}

// Called every frame
void ATopDownCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Move Player Logic
	if (CanMove)
	{
		// Validate if any keys are pressed, length of the vector will always be positive
		if (MovementDirection.Length() > 0.f)
		{
			if (MovementDirection.Length() > 1.f)
			{
				MovementDirection.Normalize();
			}
			// DeltaTime for framerate dependent
			FVector2D DistanceToMove = MovementDirection * MovementSpeed * DeltaTime;
			FVector CurrLocation = GetActorLocation();
			FVector NewLocation = CurrLocation + FVector(DistanceToMove.X, 0.0f, 0.f);
			if (!IsInMapBoundsHorizontal(NewLocation.X))
			{
				NewLocation -= FVector(DistanceToMove.X, 0.0f, 0.f);
			}
			NewLocation += FVector(0.f, 0.f, DistanceToMove.Y);
			if (!IsInMapBoundsVertical(NewLocation.Z))
			{
				NewLocation -= FVector(0.f, 0.f, DistanceToMove.Y);
			}

			SetActorLocation(NewLocation);
		}
	}
	// Rotate Gun
	APlayerController *PlayerController = Cast<APlayerController>(Controller);
	if (PlayerController)
	{
		FVector MouseWorldLocation, MouseWorldDirection;
		// We need to get the mouse position and set to world position to use in RFL function
		PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

		// Can use gun parnet location but this is the same of the player position
		FVector CurrentLocation = GetActorLocation();
		// We specifically set the y value to 0 b/c we dont care for this axis in 2D game not needed to be use in calculation
		FVector Start = FVector(CurrentLocation.X, 0.0f, CurrentLocation.Z);
		FVector Target = FVector(MouseWorldLocation.X, 0.0f, MouseWorldLocation.Z);

		FRotator GunParentRotator = UKismetMathLibrary::FindLookAtRotation(Start, Target);

		GunParent->SetRelativeRotation(GunParentRotator);
	}
}

// Called to bind functionality to input, this function will be called in Unreal, inside this function
// we bind the input actions to the functions
void ATopDownCharacter::SetupPlayerInputComponent(UInputComponent *PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	UEnhancedInputComponent *EnhanceInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent);
	if (EnhanceInputComp)
	{
		// 'this' is passed as the function we like to connection exist in this class
		EnhanceInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ATopDownCharacter::MoveTriggered);
		EnhanceInputComp->BindAction(MoveAction, ETriggerEvent::Completed, this, &ATopDownCharacter::MoveCompleted);
		EnhanceInputComp->BindAction(MoveAction, ETriggerEvent::Canceled, this, &ATopDownCharacter::MoveCompleted);

		// Started is used when key is pressed just once
		EnhanceInputComp->BindAction(FireAction, ETriggerEvent::Started, this, &ATopDownCharacter::Fire);
		// Triggered is used when key is held down, we want the player to tap or hold down to shoot
		EnhanceInputComp->BindAction(FireAction, ETriggerEvent::Triggered, this, &ATopDownCharacter::Fire);
	}
}

// Value passed in is the vector2D we set in our inputAction
void ATopDownCharacter::MoveTriggered(const FInputActionValue &Value)
{
	FVector2D MoveActionValue = Value.Get<FVector2D>();
	if (CanMove)
	{
		MovementDirection = MoveActionValue;
		// Set changes the flipbook
		CharacterFlipbook->SetFlipbook(RunFlipbook);

		FVector FlipbookScale = CharacterFlipbook->GetComponentScale();
		// A key is pressed
		if (MovementDirection.X < 0.f)
		{
			// Check if we are pointed in the right direction if not correct it
			if (FlipbookScale.X > 0.f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(-1.f, 1.f, 1.f));
			}
		}
		else if (MovementDirection.X > 0.f)
		{
			if (FlipbookScale.X < 0.f)
			{
				CharacterFlipbook->SetWorldScale3D(FVector(1.f, 1.f, 1.f));
			}
		}
	}
	// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::White, MoveActionValue.ToString());
}

// This function is needed b/c we need to change back to our idle event, we don't key of the value here as
// The keys are not being pressed anymore
void ATopDownCharacter::MoveCompleted(const FInputActionValue &Value)
{
	MovementDirection = FVector2D(0.f, 0.f);

	if (IsAlive)
	{
		CharacterFlipbook->SetFlipbook(IdleFlipbook);
	}

	// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("Move Completed"));
}

void ATopDownCharacter::Fire(const FInputActionValue &Value)
{
	if (CanShoot)
	{
		CanShoot = false;

		// Spawn actor in world
		ABullet *Bullet = GetWorld()->SpawnActor<ABullet>(BulletActorToSpawn, BulletSpawnPosition->GetComponentLocation(), FRotator(0.f, 0.f, 0.f));
		if (Bullet)
		{
			// Get Mouse world locations
			APlayerController *PlayerController = Cast<APlayerController>(Controller);
			if (PlayerController)
			{
				FVector MouseWorldLocation, MouseWorldDirection;
				// We need to get the mouse position and set to world position to use in RFL function
				PlayerController->DeprojectMousePositionToWorld(MouseWorldLocation, MouseWorldDirection);

				// Calculate Bullet Direction, could be gun parent location as well same line
				FVector CurrentLocation = GetActorLocation();
				FVector2D BulletDirection = FVector2D(MouseWorldLocation.X - CurrentLocation.X, MouseWorldLocation.Z - CurrentLocation.Z);
				BulletDirection.Normalize();

				float BulletSpeed = 400.f;
				Bullet->Launch(BulletDirection, BulletSpeed);
			}
			GetWorldTimerManager().SetTimer(ShootCoolDownTimer, this, &ATopDownCharacter::OnShootCoolDownTimerTimeout, 1.0f, false, ShootCoolDownDurationInSeconds);
		}
		// GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, TEXT("Shoot"));
	}
}

void ATopDownCharacter::OnShootCoolDownTimerTimeout()
{
	if (IsAlive)
	{
		CanShoot = true;
	}
}

void ATopDownCharacter::OverlapBegin(UPrimitiveComponent *OverLappedComp, AActor *OtherActor, UPrimitiveComponent *OtherComp,
									 int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	AEnemy *Enemy = Cast<AEnemy>(OtherActor);
	if (Enemy && IsAlive)
	{
		if (DeathSound)
		{
			UGameplayStatics::PlaySound2D(this, DeathSound);
		}
		IsAlive = false;
		CanMove = false;
		CanShoot = false;

		PlayerDiedDelegate.Broadcast();
	}
}