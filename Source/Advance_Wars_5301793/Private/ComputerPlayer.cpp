// Fill out your copyright notice in the Description page of Project Settings.
#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Kismet/GameplayStatics.h"

AComputerPlayer::AComputerPlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    GameIstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
}

void AComputerPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Ottieni un riferimento a GameField (assicurati che sia inizializzato correttamente)
    /*
    //  NON SO BENE A CHE SERVA  TOLTA PER DEBUGGARE SPAWN DEI GIOCATORI NEL GAMEMODE.CPP IL 10/03
     GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!GameField)
    {
        UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Impossibile trovare GameField!"));
    }
    */
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

void AComputerPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AComputerPlayer::MakeMove()
{
    // Logica dell'IA (usa GameField, GameInstance e GameMode per prendere decisioni)
    // ...

    AAWGameMode* GameMode = (AAWGameMode*)(GetWorld()->GetAuthGameMode());

    // Alla fine del turno, chiama la funzione EndTurn() di GameMode
    GameMode->EndTurn();
}

