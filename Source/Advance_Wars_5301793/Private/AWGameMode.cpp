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
    Players.Add(AIPlayer); 


    ChoosePlayerAndStartGame();
}

void AAWGameMode::ChoosePlayerAndStartGame()
{
    //CurrentPlayer = FMath::RandRange(0, Players.Num() - 1);
    //temporaneamente faccio cosi 
    CurrentPlayer = 0;

    
    MoveCounter = 0 ;
    Players[CurrentPlayer]->OnTurn();
}

void AAWGameMode::GetNextPlayer()
{
    CurrentPlayer = (CurrentPlayer + 1) % 2;
    
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
    MoveCounter ++;
 
    GetNextPlayer();
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

    AActor* UnitToSpawn = nullptr;

    if (PlayerNumber == 0) // Giocatore Umano
    {
        if (UnitsPlaced == 0) // Primo brawler umano
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Brawler>(BrawlerClassHuman, GridPosition + FVector(0,0,2), FRotator::ZeroRotator);
        }
        else if (UnitsPlaced == 2) // Sniper umano
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Sniper>(SniperClassHuman, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
    }
    else if (PlayerNumber == 1) // Giocatore AI
    {
        if (UnitsPlaced == 1) // Brawler AI
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Brawler>(BrawlerClassAI, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
        else if (UnitsPlaced == 3) // Sniper AI
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Sniper>(SniperClassAI, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
    }

    if (UnitToSpawn)
    {
        UnitToSpawn->SetActorScale3D(FVector(1.0f, 1.0f, 0.2f));


        if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
        {
            Brawler->OwnerPlayerId = PlayerNumber;
        }
        else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
        {
            Sniper->OwnerPlayerId = PlayerNumber;
        }
        UE_LOG(LogTemp, Error, TEXT("UnitsPlaced address: %p, value before increment: %d"), &UnitsPlaced, UnitsPlaced);

        UnitsPlaced++;

        // Segna la tile come occupata
        if (GameField)
        {
           

            FVector2D GridPosition2D = GameField->GetXYPositionByRelativeLocation(GridPosition);

            GameField->SetGridCellOccupied(GridPosition2D, PlayerNumber);

            // Ottieni la Tile corrispondente alla GridPosition
            ATile* Tile = GameField->GetTile(GridPosition2D.X, GridPosition2D.Y);

            if (Tile)
            {
                // Attacca l'unità alla Tile
                
                UnitToSpawn->AttachToActor(Tile, FAttachmentTransformRules::KeepWorldTransform);

                 //If it is a Brawler
                if (AAW_Brawler* BrawlerSoldier = Cast<AAW_Brawler>(UnitToSpawn))
                {
                    BrawlerSoldier->CurrentTile = Tile; // Store the tile reference
                }
                //If it is a Sniper
                else if (AAW_Sniper* SniperSoldier = Cast<AAW_Sniper>(UnitToSpawn))
                {
                    SniperSoldier->CurrentTile = Tile; // Store the tile reference
                }
            }
            else
            {
                UE_LOG(LogTemp, Error, TEXT("No Tile found at GridPosition: %s"), *GridPosition.ToString());
                // Gestisci l'errore: la Tile non è stata trovata
                // Potresti voler distruggere l'unità spawnata o fare altro...
            }

        }

       

        if (UnitsPlaced >= 4)
        {
            bIsPlacementPhaseOver = true;
           
        }

        EndTurn();
    }
}


void AAWGameMode::SetSelectedTile(const FVector2D Position) const
{
    // Reset the field colors
    GameField->ResetGameStatusField();

    // Show selected Tile (blue color)
    ATile* SelectedTile = GameField->GetTile(Position.X, Position.Y);
    if (SelectedTile)
    {
        //GameField->GetTile(Position.X, Position.Y);

        // Get the unit on the selected tile
        AActor* Unit = SelectedTile->GetUnit();     // TODO : assicurati che l'unita venga attachata alla tile correttamente 

        if (Unit && Unit->Implements<UAW_BaseSoldier>())
        {
            IAW_BaseSoldier* Soldier = Cast<IAW_BaseSoldier>(Unit);
            // Return the moves for the piece at the given position passed as a parameter
            TArray<FVector2D> LegalMoves = Soldier->GetLegalMoves();

            //memorizzo le mosse legali nel gamefield
            GameField->SetLegalMoves(LegalMoves);
        }
    }

    // Show legal moves (yellow color)
    GameField->ShowLegalMovesInTheField();
}