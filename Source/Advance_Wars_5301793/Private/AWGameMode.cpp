#include "AWGameMode.h"
#include "AWPlayerController.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "AW_Sniper.h"
#include "AW_Brawler.h" 
#include "ComputerPlayer.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"

AAWGameMode::AAWGameMode()
{
    // Imposta la classe di default per il PlayerController
    PlayerControllerClass = AAWPlayerController::StaticClass();

    // Imposta la classe di default per il Pawn
    DefaultPawnClass = AHumanPlayer::StaticClass();

    bIsPlacementPhaseOver = false;
    TotalUnitsToPlace = 4;
    UnitsPlaced = 0; 

    // Imposta la dimensione del campo di gioco
    FieldSize = 25;
}

void AAWGameMode::BeginPlay()
{
    Super::BeginPlay();
    
    MoveCounter = 0;
    bIsGameOver = false;

    AHumanPlayer* HumanPLayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();

    /*
    UWorld* World = GetWorld();
    if (!IsValid(World))
    {
        UE_LOG(LogTemp, Error, TEXT("World is not valid in BeginPlay!"));
        return; // Esci se il World non è valido.
    }
    */

    /*
    AAWPlayerController* PlayerController = World->GetFirstPlayerController();
    if (!IsValid(PlayerController))
    {
        UE_LOG(LogTemp, Error, TEXT("PlayerController is not valid in BeginPlay!"));
        return;
    }
    */

    //AHumanPlayer* HumanPLayer = PlayerController->GetPawn<AHumanPlayer>();

    if (!IsValid(HumanPLayer))
    {
        UE_LOG(LogTemp, Error, TEXT("No player paen of type '%s' was found."), *AHumanPlayer::StaticClass()->GetName());
        return;
    }
    if (GameFieldClass != nullptr)
    {
        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
        GameField->Size = FieldSize;
        
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
    }

    float CameraPosX = ((GameField->TileSize * FieldSize) + ((FieldSize - 1) * GameField->TileSize * GameField->CellPadding)) * 0.5f;
    float Zposition = 2500.0f;
    FVector CameraPos(CameraPosX, CameraPosX, Zposition);
    HumanPLayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());

    Players.Add(HumanPLayer);


    //add a computer player
    auto* AIPlayer = GetWorld()->SpawnActor<AComputerPlayer>(FVector(), FRotator());
    Players.Add(AIPlayer); // Explicitly cast to IPlayerInterface*


    ChoosePlayerAndStartGame();
}

void AAWGameMode::ChoosePlayerAndStartGame()
{
    //CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);
    //temporaneamente faccio cosi 
    CurrentPlayer = 0;


    for (int32 IndexI = 0; IndexI < Players.Num(); IndexI++)
    {
        Players[IndexI]->PlayerId = IndexI;
       
    }
    MoveCounter += 1;
    Players[CurrentPlayer]->OnTurn();
}

int32 AAWGameMode::GetNextPlayer(int32 Player)
{
    Player++;
    if(!Players.IsValidIndex(Player))
    {
        Player = 0; 
    }
    return Player;
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


void AAWGameMode::SwitchPlayer()
{
    // todo : aggiornala ma era troppo complicata prima
    MoveCounter += 1;
    CurrentPlayer = GetNextPlayer(CurrentPlayer);
    Players[CurrentPlayer]->OnTurn();

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
void AAWGameMode::SetUnitPlacement(const int32 PlayerNumber, const FVector& GridPosition)
{
    if (bIsPlacementPhaseOver || PlayerNumber != CurrentPlayer)
    {
        return;
    }

    // Definisci la classe del Blueprint dell'unità da spawnare in base al turno
    //TSubclassOf<AActor> UnitClass = (UnitsPlaced % 2 == 0) ? AAW_Brawler::StaticClass() : AAW_Sniper::StaticClass();

    // Calcola la posizione di spawn sulla griglia
    //FVector Location = GameField->GetActorLocation() + GridPosition + FVector(0, 0, 10); // Aggiungi un offset in Z se necessario

    // Spawna l'unità dal Blueprint
    AActor* NewBrawler = GetWorld()->SpawnActor(BrawlerClass, &GridPosition);

    // Imposta la rotazione dell'unità (se necessario)
    // NewUnit->SetActorRotation(...);

    // Incrementa il contatore delle unità piazzate
    UnitsPlaced++;

    // Controlla se la fase di piazzamento è terminata
    if (UnitsPlaced >= TotalUnitsToPlace)
    {
        bIsPlacementPhaseOver = true;
        // Inizia la fase di gioco vera e propria
    }

    // Termina il turno
    EndTurn();
}