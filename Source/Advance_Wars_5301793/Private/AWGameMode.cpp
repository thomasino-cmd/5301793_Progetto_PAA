#include "AWGameMode.h"
#include "AWPlayerController.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "ComputerPlayer.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AAWGameMode::AAWGameMode()
{
    // Imposta la classe di default per il PlayerController
    PlayerControllerClass = APlayerController::StaticClass();

    // Imposta la classe di default per il Pawn
    DefaultPawnClass = AHumanPlayer::StaticClass();

    // Imposta la dimensione del campo di gioco
    //FieldSize = 25;
}

void AAWGameMode::BeginPlay()
{
    Super::BeginPlay();
    //AHumanPlayer* HumanPLayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();

    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Error, TEXT("World is not valid in BeginPlay!"));
        return; // Esci se il World non è valido.
    }

    APlayerController* PlayerController = World->GetFirstPlayerController();
    if (!IsValid(PlayerController)) {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid in BeginPlay!"));
        return;
    }

    AHumanPlayer* HumanPLayer = PlayerController->GetPawn<AHumanPlayer>();

    if (!IsValid(HumanPLayer))
    {
        UE_LOG(LogTemp, Error, TEXT("No player paen of type '%s' was found."), *AHumanPlayer::StaticClass()->GetName());
        return;
    }
    if (GameFieldClass != nullptr)
    {
        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
        //USELESS : GameField->Size = FieldSize;
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
    }

    float CameraPosX = ((GameField->TileSize * FieldSize) + ((FieldSize - 1) * GameField->TileSize * GameField->CellPadding)) * 0.5f;
    float Zposition = 1000.0f;
    FVector CameraPos(CameraPosX, CameraPosX, Zposition);
    HumanPlayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

    if (HumanPLayer)
    {
       // Players.Add(HumanPLayer);
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("HumanPLayer is null!"));
    }

    //add a computer player
    auto* AIPlayer = GetWorld()->SpawnActor<AComputerPlayer>(FVector(), FRotator());
    Players.Add(Cast<IPlayerInterface>(AIPlayer)); // Explicitly cast to IPlayerInterface*


    ChoosePlayerAndStartGame();
}

void AAWGameMode::ChoosePlayerAndStartGame()
{
    CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);

    for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
    {
        Players[IndexI]->PlayerId = IndexI;
        //Players[IndexI]->Sign = IndexI == CurrentPlayer ? ESign::X : ESign::O;
    }
    MoveCounter += 1;
    Players[CurrentPlayer]->OnTurn();
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
        //CurrentPlayerPawn->MoveUnit(FromX, FromY, ToX, ToY);
    }
}

void AAWGameMode::AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    // Ottieni il giocatore corrente
    AHumanPlayer* CurrentPlayerPawn = Cast<AHumanPlayer>(UGameplayStatics::GetPlayerPawn(this, CurrentPlayer));
    if (CurrentPlayerPawn)
    {
        // Fai attaccare l'unità del giocatore
        //CurrentPlayerPawn->AttackUnit(FromX, FromY, ToX, ToY);
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
            //CurrentPlayerPawn->StartTurn();
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