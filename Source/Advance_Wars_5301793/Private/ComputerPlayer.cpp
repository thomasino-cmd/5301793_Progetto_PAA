// Fill out your copyright notice in the Description page of Project Settings.
#include "ComputerPlayer.h"
#include "Kismet/GameplayStatics.h"

AComputerPlayer::AComputerPlayer()
{
    PrimaryActorTick.bCanEverTick = true;
}

void AComputerPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Ottieni un riferimento a GameField (assicurati che sia inizializzato correttamente)
    GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!GameField)
    {
        UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Impossibile trovare GameField!"));
    }
}

void AComputerPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Implementazione delle funzioni di PlayerInterface
void AComputerPlayer::OnTurn()
{
    MakeMove();
}

void AComputerPlayer::OnWin()
{
    // ... (Logica per la vittoria del computer) ...
}

void AComputerPlayer::OnLose()
{
    // ... (Logica per la sconfitta del computer) ...
}

void AComputerPlayer::MakeMove()
{
    // Logica dell'IA (usa GameField, GameInstance e GameMode per prendere decisioni)
    // ...

    // Alla fine del turno, chiama la funzione EndTurn() di GameMode
    GetGameMode()->EndTurn();
}

UAWGameInstance* AComputerPlayer::GetGameInstance()
{
    return Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

AAWGameMode* AComputerPlayer::GetGameMode()
{
    return Cast<AAWGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
}