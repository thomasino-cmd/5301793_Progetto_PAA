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

//void AAWGameMode::BeginPlay()
//{
//    Super::BeginPlay();
//    
//    MoveCounter = 0;
//    bIsGameOver = false;
//
//    AHumanPlayer* HumanPLayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();
//
//
//    if (!IsValid(HumanPLayer))
//    {
//        UE_LOG(LogTemp, Error, TEXT("No player paen of type '%s' was found."), *AHumanPlayer::StaticClass()->GetName());
//        return;
//    }
//    if (GameFieldClass != nullptr)
//    {
//        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
//        GameField->Size = FieldSize;
//        
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("Game Field is null"));
//    }
//
//    float CameraPosX = ((GameField->TileSize * FieldSize) + ((FieldSize - 1) * GameField->TileSize * GameField->CellPadding)) * 0.5f;
//    float Zposition = 2500.0f;
//    FVector CameraPos(CameraPosX, CameraPosX, Zposition);
//    HumanPLayer->SetActorLocationAndRotation(CameraPos, FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());
//
//    Players.Add(HumanPLayer);
//
//
//    //add a computer player
//    auto* AIPlayer = GetWorld()->SpawnActor<AComputerPlayer>(FVector(), FRotator());
//    Players.Add(AIPlayer); 
//
//
//    ChoosePlayerAndStartGame();
//}





void AAWGameMode::BeginPlay()
{
    Super::BeginPlay();
    StartCoinFlip(); // Inizia il lancio della moneta PRIMA di tutto il resto
}

void AAWGameMode::StartCoinFlip()
{
    if (CoinClass) // Assicurati di avere un Blueprint della moneta assegnato in GameMode
    {
        CoinActor = GetWorld()->SpawnActor<ACoin>(CoinClass, FVector(0, 0, 300), FRotator::ZeroRotator);
        if (CoinActor)
        {
            CoinActor->OnCoinLanded.AddDynamic(this, &AAWGameMode::OnCoinFlipResult);
            CoinActor->Flip();
        }
    }
    else
    {
        // Fallback: usa un random semplice se la moneta non è assegnata
        OnCoinFlipResult(FMath::RandRange(0, 1));
    }
}







void AAWGameMode::OnCoinFlipResult(int32 StartingPlayerIndex)
{
    CurrentPlayer = StartingPlayerIndex;
    UE_LOG(LogTemp, Log, TEXT("Coin flip result: Player %d starts!"), CurrentPlayer);

    // **Ora inizializza il gioco**
    if (GameFieldClass)
    {
        GameField = GetWorld()->SpawnActor<AGameField>(GameFieldClass);
        GameField->Size = FieldSize;
    }

    //// Posiziona la telecamera, spawna giocatori, ecc. (come nel tuo codice originale)
    //AHumanPlayer* HumanPlayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();
    //if (HumanPlayer)
    //{
    //    float CameraPosX = ((GameField->TileSize * FieldSize) + ((FieldSize - 1) * GameField->TileSize * GameField->CellPadding)) * 0.5f;
    //    HumanPlayer->SetActorLocationAndRotation(FVector(CameraPosX, CameraPosX, 2500), FRotationMatrix::MakeFromX(FVector(0, 0, -1)).Rotator());
    //}

    MoveCounter = 0;
        bIsGameOver = false;
    
        AHumanPlayer* HumanPLayer = GetWorld()->GetFirstPlayerController()->GetPawn<AHumanPlayer>();
    
    
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

    Players[CurrentPlayer]->OnTurn(); // Inizia il turno del giocatore scelto
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


        if (PlayerNumber == 0) // Giocatore Umano
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
            {
                Brawler->OwnerPlayerId = PlayerNumber;
                Player1Brawlers.Add(Brawler); // Add to Player 1 Brawlers
            }
            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
            {
                Sniper->OwnerPlayerId = PlayerNumber;
                Player1Snipers.Add(Sniper); // Add to Player 1 Snipers
            }
        }
        else if (PlayerNumber == 1) // Giocatore AI
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(UnitToSpawn))
            {
                Brawler->OwnerPlayerId = PlayerNumber;
                Player2Brawlers.Add(Brawler); // Add to Player 2 Brawlers
            }
            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(UnitToSpawn))
            {
                Sniper->OwnerPlayerId = PlayerNumber;
                Player2Snipers.Add(Sniper); // Add to Player 2 Snipers
            }
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
                
                //UnitToSpawn->AttachToActor(Tile, FAttachmentTransformRules::KeepRelativeTransform);

                 //If it is a Brawler
                if (AAW_Brawler* BrawlerSoldier = Cast<AAW_Brawler>(UnitToSpawn))
                {
                    BrawlerSoldier->SetTileIsOnNow(Tile); // Store the tile reference
                }
                //If it is a Sniper
                else if (AAW_Sniper* SniperSoldier = Cast<AAW_Sniper>(UnitToSpawn))
                {
                    SniperSoldier->SetTileIsOnNow(Tile);  // Store the tile reference
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