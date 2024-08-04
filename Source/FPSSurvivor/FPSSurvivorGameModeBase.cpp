// Fill out your copyright notice in the Description page of Project Settings.
#include "Kismet/GameplayStatics.h"
#include "FPSSurvivorGameModeBase.h"

AFPSSurvivorGameModeBase::AFPSSurvivorGameModeBase()
{
    PrimaryActorTick.bCanEverTick = true;
    SetScore(0);
}

void AFPSSurvivorGameModeBase::BeginPlay()
{
    Super::BeginPlay();
    SetScore(0);
}

void AFPSSurvivorGameModeBase::SetScore(int NewScore)
{
    if (NewScore >= 0)
    {
        Score = NewScore;
        ScoreChangedDelegate.Broadcast(Score);
    }
}
void AFPSSurvivorGameModeBase::AddScore(int AmountToAdd)
{
    int NewScore = Score + AmountToAdd;
    SetScore(NewScore);
}

void AFPSSurvivorGameModeBase::RestartGame()
{
    GetWorldTimerManager().SetTimer(RestartTimer, this, &AFPSSurvivorGameModeBase::OnRestartGameTimerTimeout, 1.f, false, TimeBeforeRestart);
}

void AFPSSurvivorGameModeBase::OnRestartGameTimerTimeout()
{
    UGameplayStatics::OpenLevel(GetWorld(), FName("MainLevel"));
}