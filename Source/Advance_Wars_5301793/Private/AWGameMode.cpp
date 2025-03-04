#include "AWGameMode.h"
#include "AWPlayerController.h"
#include "HumanPlayer.h"
#include "GameField.h"
//todo include ai player
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AAWGameMode::AAWGameMode()
{
    // Imposta la classe di default per il PlayerController
    PlayerControllerClass = APlayerController::StaticClass();
    // Imposta la classe di default per il Pawn
    DefaultPawnClass = AHumanPlayer::StaticClass();
    // Imposta la dimensione del campo di gioco
    FieldSize = 25;
}

void AAWGameMode::BeginPlay()
{
    Super::BeginPlay();

    // Inizializza lo stato del gioco
    InitializeGame();

    // Crea il campo di gioco
    if (GameFieldClass != nullptr)
    {
        // Calcola la dimensione del GameField
        FVector FieldSizeVector = FVector(FieldSize * 100.0f, FieldSize * 100.0f, 0.0f);

        // Calcola la posizione del GameField
        FVector FieldLocation = FVector::ZeroVector; // Il GameField verrà generato al centro del mondo di gioco

        // Genera il GameField
        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass, FieldLocation, FRotator::ZeroRotator);
        GameField->Size = FieldSize;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
    }

    // Posiziona la camera
    if (GetWorld()->GetFirstPlayerController() != nullptr && GetWorld()->GetFirstPlayerController()->GetPawn() != nullptr)
    {
        // Calcola la posizione della camera
        FVector CameraLocation = FVector(FieldSize * 50.0f, FieldSize * 50.0f, 1000.0f); // La camera verrà posizionata sopra il GameField

        // Imposta la posizione della camera
        GetWorld()->GetFirstPlayerController()->GetPawn()->SetActorLocation(CameraLocation);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Player Controller or Pawn is null"));
    }

    // Avvia il gioco per il primo giocatore
    SwitchPlayer();
}


void AAWGameMode::EndTurn()
{
    // Cambia il turno al giocatore successivo
    SwitchPlayer();
}

void AAWGameMode::MoveUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    // Ottieni il giocatore corrente
    AHumanPlayer* CurrentPlayerPawn = Cast<AHumanPlayer>(UGameplayStatics::GetPlayerPawn(this, CurrentPlayer));
    if (CurrentPlayerPawn)
    {
        // Sposta l'unità del giocatore
        CurrentPlayerPawn->MoveUnit(FromX, FromY, ToX, ToY);
    }
}

void AAWGameMode::AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    // Ottieni il giocatore corrente
    AHumanPlayer* CurrentPlayerPawn = Cast<AHumanPlayer>(UGameplayStatics::GetPlayerPawn(this, CurrentPlayer));
    if (CurrentPlayerPawn)
    {
        // Fai attaccare l'unità del giocatore
        CurrentPlayerPawn->AttackUnit(FromX, FromY, ToX, ToY);
    }
}

void AAWGameMode::InitializeGame()
{
    // Inizializza le variabili di stato del gioco
    CurrentPlayer = 0;
    TurnNumber = 1;

    // Crea il campo di gioco
    if (GameFieldClass != nullptr)
    {
        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
        GameField->Size = FieldSize;
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
    }

    // Crea il giocatore umano
    if (HumanPlayerClass != nullptr)
    {
        HumanPlayer = GetWorld()->SpawnActor<AHumanPlayer>(HumanPlayerClass);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Human Player is null"));
    }
}

void AAWGameMode::SwitchPlayer()
{
    // Cambia il giocatore corrente
    CurrentPlayer = (CurrentPlayer + 1) % 2;

    // Incrementa il numero del turno se il giocatore corrente è il primo giocatore
    if (CurrentPlayer == 0)
    {
        TurnNumber++;
    }

    // Controlla se un giocatore ha vinto
    if (CheckWinCondition())
    {
        // Termina il gioco se un giocatore ha vinto
        EndGame();
    }
    else
    {
        // Altrimenti, fai iniziare il turno al giocatore corrente
        AHumanPlayer* CurrentPlayerPawn = Cast<AHumanPlayer>(UGameplayStatics::GetPlayerPawn(this, CurrentPlayer));
        if (CurrentPlayerPawn)
        {
            CurrentPlayerPawn->StartTurn();
        }
    }
}

bool AAWGameMode::CheckWinCondition()
{
    // Controlla se un giocatore ha vinto (implementazione fittizia)
    // Sostituisci questa implementazione con la tua logica di vittoria effettiva
    return false;
}

void AAWGameMode::EndGame()
{
    // Termina il gioco (implementazione fittizia)
    // Sostituisci questa implementazione con la tua logica di fine gioco effettiva
}