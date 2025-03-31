#include "AWGameMode.h"
#include "AWPlayerController.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "AW_Sniper.h"
#include "AW_Brawler.h" 
#include "ComputerPlayer.h"
#include "EngineUtils.h"
#include "Kismet/GameplayStatics.h"
#include "InGameHUDWidget.h"
#include "AStarComputerPlayer.h"




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
    MoveHistoryManager = NewObject<UMoveHistoryManager>(this, TEXT("MoveHistoryManager"));
    StartGameSequence();
   
}

bool AAWGameMode::IsInCoinTossPhase() const
{
    return bInCoinTossPhase;
}

void AAWGameMode::StartGameSequence()
{
    if (ChooseAIWidgetClass)
    {
        ChooseAIWidget = CreateWidget<UChooseAIWidget>(GetWorld(), ChooseAIWidgetClass);
        if (ChooseAIWidget)
        {
            ChooseAIWidget->AddToViewport();
            
        }
    }


   // NON QUI  StartGameAfterSelection();
    

}

void AAWGameMode::SetAIType(bool bUseAStar)
{
    bUseAStarAI = bUseAStar;
}

void AAWGameMode::StartGameAfterSelection()
{
    // Rimuovi il widget di selezione
    if (ChooseAIWidget)
    {
        ChooseAIWidget->RemoveFromParent();
        ChooseAIWidget = nullptr;
    }

    // Procedi con l'inizializzazione normale del gioco
    InitializeGameplay();
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

    //auto* AIPlayer = GetWorld()->SpawnActor<AComputerPlayer>();

    AComputerPlayer* AIPlayer = nullptr;
    if (bUseAStarAI)
    {
        AIPlayer = GetWorld()->SpawnActor<AAStarComputerPlayer>();
    }
    else
    {
        AIPlayer = GetWorld()->SpawnActor<AComputerPlayer>();
    }


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
    // 1. Crea e aggiungi l'HUD principale (se non esiste già)
    if (InGameHUDClass && !InGameHUD)
    {
        InGameHUD = CreateWidget<UInGameHUDWidget>(GetWorld(), InGameHUDClass);
        if (InGameHUD)
        {
            //LoadScores();
            InGameHUD->AddToViewport();
        }
    }

    // 2. Crea e aggiungi il widget dello storico delle mosse (WBP_MoveHistory)
    if (MoveHistoryWidgetClass && !MoveHistoryWidget) // Assicurati di aver definito MoveHistoryWidgetClass e MoveHistoryWidget nella GameMode
    {
        MoveHistoryWidget = CreateWidget<UMoveHistoryWidget>(GetWorld(), MoveHistoryWidgetClass);
        if (MoveHistoryWidget)
        {
            MoveHistoryWidget->AddToViewport();

            // Opzionale: Imposta una posizione o uno ZOrder specifico per sovrapporlo correttamente
            //MoveHistoryWidget->SetZOrder(10); // Più alto = più in primo piano
        }
    }

    // 3. Avvia il turno del giocatore corrente
    if (Players.IsValidIndex(CurrentPlayer))
    {
        Players[CurrentPlayer]->OnTurn();
        UpdateHUD();
    }
}




void AAWGameMode::HandleCoinFlipInput()
{
    UE_LOG(LogTemp, Warning, TEXT("HandleCoinFlipInput called. CoinActor: %p, bInCoinTossPhase: %d"),
        CoinActor, bInCoinTossPhase);

    if (CoinActor && bInCoinTossPhase)
    {
        CoinActor->LaunchCoin();
        bInCoinTossPhase = false;
    }
}



void AAWGameMode::UpdateHUD()
{
    if (!InGameHUD) return;

    // 1. Aggiorna indicatore di turno
    InGameHUD->UpdateTurnIndicator(CurrentPlayer == 0);

    // 2. Prepara dati salute in ordine: Brawler1, Sniper1, Brawler2, Sniper2
    TArray<float> CurrentHealths;
    TArray<float> MaxHealths;

    // Helper per aggiungere valori in modo sicuro
    auto AddHealthData = [](TArray<float>& Current, TArray<float>& Max, const TArray<AAW_Brawler*>& Brawlers, const TArray<AAW_Sniper*>& Snipers) {
        // Brawler
        if (Brawlers.Num() > 0) {
            Current.Add(Brawlers[0]->GetHealth());
            Max.Add(Brawlers[0]->GetMaxHealth());
        }
        else {
            Current.Add(0.f);
            Max.Add(1.f);
        }

        // Sniper
        if (Snipers.Num() > 0) {
            Current.Add(Snipers[0]->GetHealth());
            Max.Add(Snipers[0]->GetMaxHealth());
        }
        else {
            Current.Add(0.f);
            Max.Add(1.f);
        }
        };

    // Player 1 (Brawler1, Sniper1)
    AddHealthData(CurrentHealths, MaxHealths, Player1Brawlers, Player1Snipers);

    // Player 2 (Brawler2, Sniper2)
    AddHealthData(CurrentHealths, MaxHealths, Player2Brawlers, Player2Snipers);

    // 3. Invia i dati (garantiamo sempre 4 elementi)
    InGameHUD->UpdateHealthBars(CurrentHealths, MaxHealths);
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
    UpdateHUD();

    Players[CurrentPlayer]->OnTurn();

}



bool AAWGameMode::CheckWinCondition()
{
    // Controlla se un giocatore ha perso tutte le unità
    for (int32 PlayerId = 0; PlayerId < Players.Num(); PlayerId++)
    {
        TArray<AActor*> PlayerUnits = GetCurrentPlayerUnits(PlayerId);
        int32 AliveUnits = 0;

        for (AActor* Unit : PlayerUnits)
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
            {
                if (Brawler->GetHealth() > 0) AliveUnits++;
            }
            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
            {
                if (Sniper->GetHealth() > 0) AliveUnits++;
            }
        }

        if (AliveUnits == 0 && MoveCounter > 4)
        {
            WinningPlayerId = (PlayerId == 0) ? 1 : 0;  // L'altro giocatore vince
            return true;
        }
    }

    return false;
}

void AAWGameMode::EndGame()
{
    if (GameField)
    {
        GameField->ResetField();

        // Distruggi tutti gli ostacoli rimanenti
        for (TActorIterator<AActor> ActorItr(GetWorld()); ActorItr; ++ActorItr)
        {
            if (ActorItr->ActorHasTag(FName("Obstacle")))
            {
                ActorItr->Destroy();
            }
        }
    }

    // Distruggi tutte le unità rimanenti
    for (AAW_Brawler* Brawler : Player1Brawlers)
    {
        if (Brawler) Brawler->Destroy();
    }
    for (AAW_Sniper* Sniper : Player1Snipers)
    {
        if (Sniper) Sniper->Destroy();
    }
    for (AAW_Brawler* Brawler : Player2Brawlers)
    {
        if (Brawler) Brawler->Destroy();
    }
    for (AAW_Sniper* Sniper : Player2Snipers)
    {
        if (Sniper) Sniper->Destroy();
    }

    Player1Brawlers.Empty();
    Player1Snipers.Empty();
    Player2Brawlers.Empty();
    Player2Snipers.Empty();



      
    // Mostra il widget di fine partita
    if (GameOverWidgetClass)
    {
        GameOverWidget = CreateWidget<UEndGameWidget>(GetWorld(), GameOverWidgetClass);
        if (GameOverWidget)
        {
            GameOverWidget->AddToViewport();
            GameOverWidget->SetWinnerText(WinningPlayerId);

            // Imposta lo ZOrder alto per sovrapporsi a tutto
           // GameOverWidget->SetZOrder(100);
           GameOverWidget->SetAlignmentInViewport(FVector2D(0.5f, 0.5f));
        }
    }
    


    // Aggiorna lo stato del gioco
    bIsGameOver = true;
}


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



void AAWGameMode::SaveScores()
{

}

void AAWGameMode::LoadScores()
{

}


// In AAWGameMode.cpp
void AAWGameMode::LogMove(const FString& PlayerID, const FString& UnitID, const FString& FromCell, const FString& ToCell)
{
    if (MoveHistoryManager)
    {
        MoveHistoryManager->LogMove(PlayerID, UnitID, FromCell, ToCell);

        // Converti TArray<TObjectPtr<UMoveEntry>> in TArray<UObject*>
        TArray<UObject*> Entries;
        for (const auto& Entry : MoveHistoryManager->MoveHistory)
        {
            Entries.Add(Entry.Get());
        }

        // Aggiorna il widget (esempio: se hai un riferimento al widget)
        if (MoveHistoryWidget)
        {
            MoveHistoryWidget->UpdateHistory(Entries);
        }
    }
}

void AAWGameMode::LogAttack(const FString& PlayerID, const FString& UnitID, const FString& TargetCell, int32 Damage)
{
    if (MoveHistoryManager) // Stesso controllo qui
    {
        MoveHistoryManager->LogAttack(PlayerID, UnitID, TargetCell, Damage);

        // Converti TArray<TObjectPtr<UMoveEntry>> in TArray<UObject*>
        TArray<UObject*> Entries;
        for (const auto& Entry : MoveHistoryManager->MoveHistory)
        {
            Entries.Add(Entry.Get());
        }

        // Aggiorna il widget (esempio: se hai un riferimento al widget)
        if (MoveHistoryWidget)
        {
            MoveHistoryWidget->UpdateHistory(Entries);
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("MoveHistoryManager is null in LogAttack"));
    }
}



void AAWGameMode::RestartGame()
{
    // Rimuovi il widget di fine partita
    if (GameOverWidget)
    {
        GameOverWidget->RemoveFromParent();
        GameOverWidget = nullptr;
    }

    // Resetta tutte le variabili di stato
    bIsGameOver = false;
    bIsPlacementPhaseOver = false;
    bInCoinTossPhase = false; // Aggiungi questa linea
    bCoinFlipCompleted = false; // Aggiungi questa linea
    Player1UnitsPlaced = 0;
    Player2UnitsPlaced = 0;
    MoveCounter = 0;
    WinningPlayerId = 0;

    // Pulisci gli array delle unità
    Player1Brawlers.Empty();
    Player1Snipers.Empty();
    Player2Brawlers.Empty();
    Player2Snipers.Empty();

    // Distruggi la moneta esistente se presente
    if (CoinActor)
    {
        CoinActor->Destroy();
        CoinActor = nullptr;
    }

    // Ricomincia la sequenza di gioco PROPERLY
    StartGameSequence(); // Questo chiamerà InitializeGameplay e SpawnCoinForFlip
}





