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



// AWGameMode.cpp
void AAWGameMode::BeginPlay()
{
    Super::BeginPlay();
    StartGameSequence();
   
}

bool AAWGameMode::IsInCoinTossPhase() const
{
    return bInCoinTossPhase;
}

void AAWGameMode::StartGameSequence()
{
    // Fase 1: Spawn e setup degli elementi base
    InitializeGameplay();

    // Fase 2: Lancio della moneta
    SpawnCoinForFlip();
    bInCoinTossPhase = true;

}

void AAWGameMode::InitializeGameplay()
{
    // Inizializzazione base
    MoveCounter = 0;
    bIsGameOver = false;
    bCoinFlipCompleted = false;

    // Setup campo di gioco
    if (GameFieldClass)
    {
        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
        GameField->Size = FieldSize;
    }

    // Setup giocatori
    AHumanPlayer* HumanPlayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();
    if (!HumanPlayer) return;

    auto* AIPlayer = GetWorld()->SpawnActor<AComputerPlayer>();
    Players.Add(HumanPlayer);
    Players.Add(AIPlayer);

    // Posizionamento camera
    float CameraPosX = ((GameField->TileSize * FieldSize) + ((FieldSize - 1) * GameField->TileSize * GameField->CellPadding)) * 0.5f;
    HumanPlayer->SetActorLocationAndRotation(
        FVector(CameraPosX, CameraPosX, 2500.0f),
        FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator()
    );
}



void AAWGameMode::SpawnCoinForFlip()
{
    if (!CoinClass) return;

    // Calcola manualmente il centro se GameField non ha il metodo
    FVector CoinLocation = FVector::ZeroVector;
    if (GameField)
    {
        float CenterX = (GameField->TileSize * GameField->Size) / 2;
        CoinLocation = FVector(CenterX, CenterX, 50.f);
    }

    CoinActor = GetWorld()->SpawnActor<ACoin>(CoinClass, CoinLocation, FRotator::ZeroRotator);
    if (CoinActor)
    {
        // Sostituisci con il tuo sistema di callback esistente
        CoinActor->OnLanded.AddDynamic(this, &AAWGameMode::OnCoinFlipComplete);
    }
    else
    {
        OnCoinFlipComplete(FMath::RandRange(0, 1));
    }
}







void AAWGameMode::OnCoinFlipComplete(int32 StartingPlayerIndex)
{
    if (CoinActor)
    {
        CoinActor->Destroy();
        CoinActor = nullptr;
    }

    bCoinFlipCompleted = true;
    CurrentPlayer = StartingPlayerIndex;
    StartFirstTurn();
    bInCoinTossPhase = false; // Aggiungi questa linea
}



void AAWGameMode::StartFirstTurn()
{
    if (Players.IsValidIndex(CurrentPlayer))
    {
        Players[CurrentPlayer]->OnTurn();
    }
}




void AAWGameMode::HandleCoinFlipInput()
{
    if (CoinActor && bInCoinTossPhase)
    {
        // Parametri aumentati per altezza e rotazione
        CoinActor->LaunchCoin();
        bInCoinTossPhase = false;
    }
}








ACoin* AAWGameMode::GetCoinActor() const
{
    return CoinActor;
}

void AAWGameMode::GetNextPlayer()
{
    CurrentPlayer = (CurrentPlayer + 1) % 2;
    
}




void AAWGameMode::EndTurn()
{
    // First check if the game should end
    if (CheckWinCondition())
    {
        EndGame();
        return;
    }

    // If game isn't over, switch players
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
    if (MoveCounter > TotalUnitsToPlace)
    {
        // Check if Player 1 has no units left
        TArray<AActor*> Player1Units = GetCurrentPlayerUnits(0);
        if (Player1Units.Num() == 0)
        {
            // Player 1 lost, Player 2 wins
            Players[0]->OnLose();
            Players[1]->OnWin();
            bIsGameOver = true;
            return true;
        }

        // Check if Player 2 has no units left
        TArray<AActor*> Player2Units = GetCurrentPlayerUnits(1);
        if (Player2Units.Num() == 0)
        {
            // Player 2 lost, Player 1 wins
            Players[0]->OnWin();
            Players[1]->OnLose();
            bIsGameOver = true;
            return true;
        }
    }
    return false;
}

void AAWGameMode::EndGame()
{
    // Termina il gioco (implementazione fittizia)
    // Sostituisci questa implementazione con la tua logica di fine gioco effettiva
    GameField->ResetField();
}


//
//void AAWGameMode::SetUnitPlacement(const int32 PlayerNumber, const FVector& GridPosition)
//{
//    if (bIsPlacementPhaseOver || PlayerNumber != CurrentPlayer)
//    {
//        return;
//    }
//
//    AActor* UnitToSpawn = nullptr;
//
//    if (PlayerNumber == 0) // Giocatore Umano
//    {
//        if (UnitsPlaced == 0) // Primo brawler umano
//        {
//            UnitToSpawn = GetWorld()->SpawnActor<AAW_Brawler>(BrawlerClassHuman, GridPosition + FVector(0,0,2), FRotator::ZeroRotator);
//        }
//        else if (UnitsPlaced == 2) // Sniper umano
//        {
//            UnitToSpawn = GetWorld()->SpawnActor<AAW_Sniper>(SniperClassHuman, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
//        }
//    }
//    else if (PlayerNumber == 1) // Giocatore AI
//    {
//        if (UnitsPlaced == 1) // Brawler AI
//        {
//            UnitToSpawn = GetWorld()->SpawnActor<AAW_Brawler>(BrawlerClassAI, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
//      
//        }
//        else if (UnitsPlaced == 3) // Sniper AI
//        {
//            UnitToSpawn = GetWorld()->SpawnActor<AAW_Sniper>(SniperClassAI, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
//           
//        }
//    }
//
//    if (UnitToSpawn)
//    {
//        UnitToSpawn->SetActorScale3D(FVector(1.0f, 1.0f, 0.2f));
//
//
//        if (PlayerNumber == 0) // Giocatore Umano
//        {
//            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
//            {
//                Brawler->OwnerPlayerId = PlayerNumber;
//                Player1Brawlers.Add(Brawler); // Add to Player 1 Brawlers
//            }
//            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
//            {
//                Sniper->OwnerPlayerId = PlayerNumber;
//                Player1Snipers.Add(Sniper); // Add to Player 1 Snipers
//            }
//        }
//        else if (PlayerNumber == 1) // Giocatore AI
//        {
//            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
//            {
//                Brawler->OwnerPlayerId = PlayerNumber;
//                Player2Brawlers.Add(Brawler); // Add to Player 2 Brawlers
//            }
//            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
//            {
//                Sniper->OwnerPlayerId = PlayerNumber;
//                Player2Snipers.Add(Sniper); // Add to Player 2 Snipers
//            }
//        }
//        UE_LOG(LogTemp, Error, TEXT("UnitsPlaced address: %p, value before increment: %d"), &UnitsPlaced, UnitsPlaced);
//
//        UnitsPlaced++;
//
//        // Segna la tile come occupata
//        if (GameField)
//        {
//           
//
//            FVector2D GridPosition2D = GameField->GetXYPositionByRelativeLocation(GridPosition);
//
//            GameField->SetGridCellOccupied(GridPosition2D, PlayerNumber);
//
//            // Ottieni la Tile corrispondente alla GridPosition
//            ATile* Tile = GameField->GetTile(GridPosition2D.X, GridPosition2D.Y);
//
//            if (Tile)
//            {
//                // Attacca l'unità alla Tile
//                
//                //UnitToSpawn->AttachToActor(Tile, FAttachmentTransformRules::KeepRelativeTransform);
//
//                 //If it is a Brawler
//                if (AAW_Brawler* BrawlerSoldier = Cast<AAW_Brawler>(UnitToSpawn))
//                {
//                    BrawlerSoldier->SetTileIsOnNow(Tile); // Store the tile reference
//                }
//                //If it is a Sniper
//                else if (AAW_Sniper* SniperSoldier = Cast<AAW_Sniper>(UnitToSpawn))
//                {
//                    SniperSoldier->SetTileIsOnNow(Tile);  // Store the tile reference
//                }
//            }
//            else
//            {
//                UE_LOG(LogTemp, Error, TEXT("No Tile found at GridPosition: %s"), *GridPosition.ToString());
//                // Gestisci l'errore: la Tile non è stata trovata
//                // Potresti voler distruggere l'unità spawnata o fare altro...
//            }
//
//        }
//
//       
//
//        if (UnitsPlaced >= 4)
//        {
//            bIsPlacementPhaseOver = true;
//           
//        }
//
//        EndTurn();
//    }
//}
//

void AAWGameMode::SetUnitPlacement(const int32 PlayerNumber, const FVector& GridPosition)
{
    if (bIsPlacementPhaseOver || PlayerNumber != CurrentPlayer)
    {
        return;
    }

    AActor* UnitToSpawn = nullptr;

    // Determina quale tipo di unità piazzare in base a quante ne ha già piazzate questo giocatore
    int32 PlayerUnitsPlaced = (PlayerNumber == 0) ? Player1UnitsPlaced : Player2UnitsPlaced;

    if (PlayerNumber == 0) // Giocatore Umano
    {
        if (PlayerUnitsPlaced == 0) // Primo brawler umano
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Brawler>(BrawlerClassHuman, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
        else if (PlayerUnitsPlaced == 1) // Sniper umano
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Sniper>(SniperClassHuman, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
    }
    else if (PlayerNumber == 1) // Giocatore AI
    {
        if (PlayerUnitsPlaced == 0) // Brawler AI
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Brawler>(BrawlerClassAI, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
        else if (PlayerUnitsPlaced == 1) // Sniper AI
        {
            UnitToSpawn = GetWorld()->SpawnActor<AAW_Sniper>(SniperClassAI, GridPosition + FVector(0, 0, 2), FRotator::ZeroRotator);
        }
    }

    if (UnitToSpawn)
    {
        UnitToSpawn->SetActorScale3D(FVector(1.0f, 1.0f, 0.2f));

        if (PlayerNumber == 0) // Giocatore Umano
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
            {
                Brawler->OwnerPlayerId = PlayerNumber;
                Player1Brawlers.Add(Brawler);
                Player1UnitsPlaced++;
            }
            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
            {
                Sniper->OwnerPlayerId = PlayerNumber;
                Player1Snipers.Add(Sniper);
                Player1UnitsPlaced++;
            }
        }
        else if (PlayerNumber == 1) // Giocatore AI
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
            {
                Brawler->OwnerPlayerId = PlayerNumber;
                Player2Brawlers.Add(Brawler);
                Player2UnitsPlaced++;
            }
            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
            {
                Sniper->OwnerPlayerId = PlayerNumber;
                Player2Snipers.Add(Sniper);
                Player2UnitsPlaced++;
            }
        }

        // Segna la tile come occupata
        if (GameField)
        {
            FVector2D GridPosition2D = GameField->GetXYPositionByRelativeLocation(GridPosition);
            GameField->SetGridCellOccupied(GridPosition2D, PlayerNumber);

            ATile* Tile = GameField->GetTile(GridPosition2D.X, GridPosition2D.Y);
            if (Tile)
            {
                if (AAW_Brawler* BrawlerSoldier = Cast<AAW_Brawler>(UnitToSpawn))
                {
                    BrawlerSoldier->SetTileIsOnNow(Tile);
                }
                else if (AAW_Sniper* SniperSoldier = Cast<AAW_Sniper>(UnitToSpawn))
                {
                    SniperSoldier->SetTileIsOnNow(Tile);
                }
            }
        }

        // Controlla se entrambi i giocatori hanno piazzato tutte le unità
        if (Player1UnitsPlaced >= 2 && Player2UnitsPlaced >= 2)
        {
            bIsPlacementPhaseOver = true;
        }

        EndTurn();
    }
}

TArray<AActor*> AAWGameMode::GetCurrentPlayerUnits(int32 PlayerId)
{
    TArray<AActor*> PlayerUnits;

    if (PlayerId == 0) // Human Player
    {
        for (AAW_Brawler* Brawler : Player1Brawlers)
        {
            if (IsValid(Brawler))
            {
                PlayerUnits.Add(Brawler);
            }
        }
        for (AAW_Sniper* Sniper : Player1Snipers)
        {
            if (IsValid(Sniper))
            {
                PlayerUnits.Add(Sniper);
            }
        }
    }
    else if (PlayerId == 1) // AI Player
    {
        for (AAW_Brawler* Brawler : Player2Brawlers)
        {
            if (IsValid(Brawler))
            {
                PlayerUnits.Add(Brawler);
            }
        }
        for (AAW_Sniper* Sniper : Player2Snipers)
        {
            if (IsValid(Sniper))
            {
                PlayerUnits.Add(Sniper);
            }
        }
    }

    return PlayerUnits;
}