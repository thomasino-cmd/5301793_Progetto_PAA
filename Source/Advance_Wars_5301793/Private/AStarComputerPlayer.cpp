// AStarComputerPlayer.cpp
#include "AStarComputerPlayer.h"
#include "GameField.h"
#include "Tile.h"

#include "AWGameMode.h"

ATile* AAStarComputerPlayer::FindOptimalTargetTile(AActor* Unit, TArray<ATile*>& ReachableTiles)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField) return nullptr;

    // Trova tutte le unità nemiche
    TArray<AActor*> EnemyUnits = GameMode->GetCurrentPlayerUnits(1 - PlayerId);

    ATile* BestTile = nullptr;
    float BestScore = MAX_FLT;

    for (ATile* Tile : ReachableTiles)
    {
        for (AActor* Enemy : EnemyUnits)
        {
            if (ATile* EnemyTile = Cast<ATile>(Enemy->GetOwner()))
            {
                TArray<ATile*> Path = AStarPathfinding(Tile, EnemyTile);
                if (Path.Num() > 0)
                {
                    float Distance = Path.Num();
                    float Danger = 0.0f; // Eventuali penalità
                    float TotalScore = Distance + Danger;

                    if (TotalScore < BestScore)
                    {
                        BestScore = TotalScore;
                        BestTile = Tile;
                    }
                }
            }
        }
    }

    return BestTile ? BestTile : (ReachableTiles.Num() > 0 ? ReachableTiles[FMath::RandRange(0, ReachableTiles.Num() - 1)] : nullptr);
}

TArray<ATile*> AAStarComputerPlayer::AStarPathfinding(ATile* StartTile, ATile* TargetTile)
{
    TArray<ATile*> Path;
    if (!StartTile || !TargetTile) return Path;

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    AGameField* GameField = GameMode->GameField;

    // Implementazione semplificata dell'algoritmo A*
    // Struttura interna per il nodo
    struct FAStarNode
    {
        ATile* Tile;
        float GCost;
        float HCost;
        float FCost() const { return GCost + HCost; }
        FAStarNode* Parent;

        FAStarNode(ATile* InTile) : Tile(InTile), GCost(FLT_MAX), HCost(0.f), Parent(nullptr) {}
    };

    TArray<FAStarNode*> OpenSet;
    TArray<FAStarNode*> AllNodes;
    TMap<ATile*, FAStarNode*> NodeMap;

    auto GetNodeForTile = [&](ATile* Tile) -> FAStarNode*
        {
            if (NodeMap.Contains(Tile))
            {
                return NodeMap[Tile];
            }
            FAStarNode* NewNode = new FAStarNode(Tile);
            NodeMap.Add(Tile, NewNode);
            AllNodes.Add(NewNode);
            return NewNode;
        };

    FAStarNode* StartNode = GetNodeForTile(StartTile);
    StartNode->GCost = 0.f;
    StartNode->HCost = FVector::Dist(StartTile->GetActorLocation(), TargetTile->GetActorLocation());
    OpenSet.Add(StartNode);

    while (OpenSet.Num() > 0)
    {
        // Seleziona il nodo con FCost minore
        FAStarNode* CurrentNode = OpenSet[0];
        for (FAStarNode* Node : OpenSet)
        {
            if (Node->FCost() < CurrentNode->FCost())
            {
                CurrentNode = Node;
            }
        }

        if (CurrentNode->Tile == TargetTile)
        {
            // Ricostruisci il percorso
            FAStarNode* Node = CurrentNode;
            while (Node)
            {
                Path.Insert(Node->Tile, 0);
                Node = Node->Parent;
            }
            break;
        }

        OpenSet.Remove(CurrentNode);

        // Per ogni Tile adiacente, se raggiungibile, aggiorna i costi
        TArray<ATile*> Neighbors = GameField->GetNeighbors(CurrentNode->Tile);

        for (ATile* NeighborTile : Neighbors)
        {
            // Verifica se la tile è camminabile
            if (!NeighborTile->IsWalkable())
            {
                continue;
            }

            FAStarNode* NeighborNode = GetNodeForTile(NeighborTile);
            float TentativeGCost = CurrentNode->GCost + FVector::Dist(CurrentNode->Tile->GetActorLocation(), NeighborTile->GetActorLocation());
            if (TentativeGCost < NeighborNode->GCost)
            {
                NeighborNode->Parent = CurrentNode;
                NeighborNode->GCost = TentativeGCost;
                NeighborNode->HCost = FVector::Dist(NeighborTile->GetActorLocation(), TargetTile->GetActorLocation());
                if (!OpenSet.Contains(NeighborNode))
                {
                    OpenSet.Add(NeighborNode);
                }
            }
        }
    }
    return Path;

}

void AAStarComputerPlayer::OnTurn()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;

    if (!GameMode->bIsPlacementPhaseOver)
    {
        // Fase di posizionamento (ereditata dalla classe base)
        Super::PlaceUnit();
    }
    else
    {
        // Fase di combattimento con logica A*
        CurrentState = EComputerMoveState::Moving;
        MakeMove();
    }
}

void AAStarComputerPlayer::MakeMove()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("AAStarComputerPlayer::MakeMove - GameMode nullo"));
        return;
    }

    AIUnits = GameMode->GetCurrentPlayerUnits(PlayerId);

    if (AIUnits.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AAStarComputerPlayer: Nessuna unità AI disponibile. Termino turno."));
        GameMode->EndTurn();
        return;
    }

    // Reset dei flag per il movimento
    bBrawlerMoved = false;
    bSniperMoved = false;
    bBrawlerAttackedThisTurn = false;
    bSniperAttackedThisTurn = false;

    // Verifica se ci sono Brawler e Sniper
    bool HasBrawler = false;
    bool HasSniper = false;

    for (AActor* Unit : AIUnits)
    {
        if (Cast<AAW_Brawler>(Unit))
        {
            HasBrawler = true;
        }
        else if (Cast<AAW_Sniper>(Unit))
        {
            HasSniper = true;
        }
    }

    // Seleziona la migliore unità da muovere invece di una casuale
    AAW_Brawler* BestBrawler = nullptr;
    AAW_Sniper* BestSniper = nullptr;

    for (AActor* Unit : AIUnits)
    {
        if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
        {
            BestBrawler = Brawler; // Qui potresti implementare una logica più avanzata per scegliere il miglior Brawler
        }
        else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
        {
            BestSniper = Sniper; // Anche qui potresti decidere un criterio migliore per scegliere il miglior Sniper
        }
    }

    if (BestBrawler && !bBrawlerMoved)
    {
        SelectedBrawler = BestBrawler;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("AI: Selezionato Brawler per movimento A*"));

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AAStarComputerPlayer::MoveBrawler, 1.0f, false);
    }
    else if (BestSniper && !bSniperMoved)
    {
        SelectedSniper = BestSniper;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("AI: Selezionato Sniper per movimento A*"));

        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AAStarComputerPlayer::MoveSniper, 1.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AAStarComputerPlayer: Nessuna unità selezionabile per il movimento."));
        GameMode->EndTurn();
    }

    // Se non ci sono Brawler o Sniper, imposta i flag a true
    if (!HasBrawler)
    {
        bBrawlerMoved = true;
        bBrawlerAttackedThisTurn = true;
    }
    if (!HasSniper)
    {
        bSniperMoved = true;
        bSniperAttackedThisTurn = true;
    }
}

void AAStarComputerPlayer::ContinueWithNextUnit_AStar()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("AAStarComputerPlayer::ContinueWithNextUnit_AStar - GameMode nullo"));
        return;
    }

    // Se entrambe le unità hanno mosso e attaccato, termina il turno
    if (bBrawlerMoved && bSniperMoved && bBrawlerAttackedThisTurn && bSniperAttackedThisTurn)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI A*: Turno completato"));
        GameMode->EndTurn();
        return;
    }

    // Se il Brawler ha mosso ma non ha ancora attaccato, prova ad attaccare
    if (bBrawlerMoved && !bBrawlerAttackedThisTurn)
    {
        if (SelectedBrawler)
        {
            PerformAttack(SelectedBrawler);
            return;
        }
    }

    // Se lo Sniper ha mosso ma non ha ancora attaccato, prova ad attaccare
    if (bSniperMoved && !bSniperAttackedThisTurn)
    {
        if (SelectedSniper)
        {
            PerformAttack(SelectedSniper);
            return;
        }
    }

    /*
    
      // Se il Brawler non ha ancora mosso, muovilo
    if (!bBrawlerMoved)
    {
        for (AActor* Unit : AIUnits)
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
            {
                SelectedBrawler = Brawler;
                FTimerHandle TimerHandle;
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveBrawler, 1.0f, false);
                return;
            }
        }
    }

    // Se lo Sniper non ha ancora mosso, muovilo
    if (!bSniperMoved)
    {
        for (AActor* Unit : AIUnits)
        {
            if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
            {
                SelectedSniper = Sniper;
                FTimerHandle TimerHandle;
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveSniper, 1.0f, false);
                return;
            }
        }
    }
    
    
    */


    // Se il Brawler non ha ancora mosso, muovilo
    if (!bBrawlerMoved)
    {
        for (AActor* Unit : AIUnits)
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
            {
                SelectedBrawler = Brawler;
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("AI A*: Passo al prossimo Brawler"));
                FTimerHandle TimerHandle;
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AAStarComputerPlayer::MoveBrawler, 1.0f, false);
                return;
            }
        }
    }

    // Se lo Sniper non ha ancora mosso, muovilo
    if (!bSniperMoved)
    {
        for (AActor* Unit : AIUnits)
        {
            if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
            {
                SelectedSniper = Sniper;
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("AI A*: Passo al prossimo Sniper"));
                FTimerHandle TimerHandle;
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AAStarComputerPlayer::MoveSniper, 1.0f, false);
                return;
            }
        }
    }
}











ATile* AAStarComputerPlayer::FindTileToGetCloserToEnemy(AAW_Brawler* Brawler)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!Brawler || !GameMode || !GameMode->GameField) return nullptr;

    TArray<AActor*> EnemyUnits = GameMode->GetCurrentPlayerUnits(0);
    ATile* BestTile = nullptr;
    float BestPathLength = MAX_FLT;
    ATile* CurrentTile = Brawler->GetTileIsOnNow(); // <--- Correzione qui

    for (AActor* Enemy : EnemyUnits)
    {
        ATile* EnemyTile = nullptr;
        if (AAW_Brawler* EnemyBrawler = Cast<AAW_Brawler>(Enemy))
        {
            EnemyTile = EnemyBrawler->GetTileIsOnNow();
        }
        else if (AAW_Sniper* EnemySniper = Cast<AAW_Sniper>(Enemy))
        {
            EnemyTile = EnemySniper->GetTileIsOnNow();
        }

        if (!EnemyTile) continue;

        TArray<ATile*> FullPath = AStarPathfinding(CurrentTile, EnemyTile);

        if (FullPath.Num() > 0)
        {
            for (int32 i = FMath::Min(FullPath.Num() - 1, Brawler->GetMovementRange()); i >= 0; i--)
            {
                if (Brawler->TilesCanReach.Contains(FullPath[i]))
                {
                    float NewPathLength = AStarPathfinding(FullPath[i], EnemyTile).Num();
                    if (NewPathLength < BestPathLength)
                    {
                        BestPathLength = NewPathLength;
                        BestTile = FullPath[i];
                    }
                    break;
                }
            }
        }
    }

    if (!BestTile) // Fallback
    {
        float MinDirectDistance = MAX_FLT;
        for (AActor* Enemy : EnemyUnits)
        {
            ATile* EnemyTile = nullptr;
            if (auto Soldier = Cast<IAW_BaseSoldier>(Enemy)) 
            {
                EnemyTile = Soldier->GetTileIsOnNow();
            }

            if (EnemyTile)
            {
                for (ATile* ReachableTile : Brawler->TilesCanReach)
                {
                    float Distance = FVector::Distance(ReachableTile->GetActorLocation(), EnemyTile->GetActorLocation());
                    if (Distance < MinDirectDistance)
                    {
                        MinDirectDistance = Distance;
                        BestTile = ReachableTile;
                    }
                }
            }
        }
    }

    return BestTile;
}









// Funzione per verificare se il Brawler può attaccare
bool AAStarComputerPlayer::CanBrawlerAttack(AAW_Brawler* Brawler)
{
    // Verifica se il Brawler ha nemici a portata di tiro
    TArray<ATile*> AttackableTiles = Brawler->GetAttackableTiles();
    for (ATile* Tile : AttackableTiles)
    {
        if (Tile->GetTileOwner() == 0)  // Verifica se il tile contiene un nemico
        {
            return true;
        }
    }

    return false;
}




void AAStarComputerPlayer::MoveBrawler()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!SelectedBrawler || !GameMode || !GameMode->GameField) return;

    // 1. Ottieni tile raggiungibili
    SelectedBrawler->TilesCanReach = SelectedBrawler->GetReachableTiles(SelectedBrawler->GetMovementRange());
    GameMode->GameField->HighlightReachableTiles(SelectedBrawler->TilesCanReach);

    // 2. Trova la tile ottimale (con pathfinding A*)
    ATile* TargetTile = FindOptimalAttackTile(SelectedBrawler);

    // 3. Se non trovato un bersaglio valido, cerca di avvicinarsi al nemico (ma senza muoversi a caso)
    if (!TargetTile)
    {
        // Se il Brawler non ha trovato una posizione di attacco ottimale, avvicinati comunque
        TargetTile = FindTileToGetCloserToEnemy(SelectedBrawler);
    }

    // 4. Se una tile è stata trovata, muovi il Brawler
    if (TargetTile)
    {
        SelectedBrawler->MoveUnit(TargetTile);

        // Setup timer per completamento movimento
        GetWorld()->GetTimerManager().SetTimer(AStarBrawlerTimerHandle, [this, GameMode]()
            {
                if (!SelectedBrawler->bIsMoving)
                {
                    bBrawlerMoved = true;
                    GetWorldTimerManager().ClearTimer(AStarBrawlerTimerHandle);
                    GameMode->GameField->ClearHighlightedTiles(SelectedBrawler->TilesCanReach);
                    ExecuteAttack(SelectedBrawler);  // Attacca non appena il Brawler è arrivato
                    ContinueWithNextUnit_AStar();  // Passa al prossimo unità da muovere
                }
            }, 1.0f, true);
    }

}



void AAStarComputerPlayer::MoveSniper()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!SelectedSniper || !GameMode || !GameMode->GameField) return;

    // 1. Ottieni tile raggiungibili
    SelectedSniper->TilesCanReach = SelectedSniper->GetReachableTiles(SelectedSniper->GetMovementRange());
    GameMode->GameField->HighlightReachableTiles(SelectedSniper->TilesCanReach);

    // 2. Trova la tile ottimale (con pathfinding A*)
    ATile* TargetTile = FindOptimalAttackTile(SelectedSniper);

    // 3. Se non trovato un bersaglio valido, cerca di avvicinarsi al nemico
    if (!TargetTile)
    {
        TargetTile = FindNearestTileToEnemy(SelectedSniper);
    }

    // 4. Se una tile è stata trovata, muovi lo Sniper
    if (TargetTile)
    {
        SelectedSniper->MoveUnit(TargetTile);

        // Setup timer per completamento movimento
        GetWorld()->GetTimerManager().SetTimer(AStarSniperTimerHandle, [this, GameMode]()
            {
                if (!SelectedSniper->bIsMoving)
                {
                    bSniperMoved = true;
                    GetWorldTimerManager().ClearTimer(AStarSniperTimerHandle);
                    GameMode->GameField->ClearHighlightedTiles(SelectedSniper->TilesCanReach);

                    ExecuteAttack(SelectedSniper);
                    
                    ContinueWithNextUnit_AStar();  // Passa al prossimo unità da muovere
                }
            }, 1.0f, true);
    }
}





// Funzione per verificare se lo Sniper è in grado di sparare
bool AAStarComputerPlayer::CanSniperAttack(AAW_Sniper* Sniper)
{
    // Verifica se lo Sniper ha nemici a portata di tiro
    TArray<ATile*> AttackableTiles = Sniper->GetAttackableTiles();
    for (ATile* Tile : AttackableTiles)
    {
        if (Tile->GetTileOwner() == 0)  // Verifica se il tile contiene un nemico
        {
            return true;
        }
    }

    return false;
}







ATile* AAStarComputerPlayer::FindOptimalAttackTile(AActor* Unit)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField) return nullptr;

    TArray<AActor*> EnemyUnits = GameMode->GetCurrentPlayerUnits(0);
    ATile* BestTile = nullptr;
    float BestScore = -MAX_FLT;

    // Ottieni le tile raggiungibili
    TArray<ATile*> ReachableTiles;
    if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
    {
        ReachableTiles = Brawler->TilesCanReach;
    }
    else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
    {
        ReachableTiles = Sniper->TilesCanReach;
    }
    else
    {
        return nullptr;
    }

    for (AActor* Enemy : EnemyUnits)
    {
        ATile* EnemyTile = nullptr;
        if (auto Soldier = Cast<IAW_BaseSoldier>(Enemy)) 
        {
            EnemyTile = Soldier->GetTileIsOnNow();
        }
        if (!EnemyTile) continue;
       
        for (ATile* ReachableTile : ReachableTiles)
        {
            float Score = 0.f;

            // 1. Calcola la distanza dalla tile raggiungibile al nemico
            float Distance = FVector::Dist(ReachableTile->GetActorLocation(), EnemyTile->GetActorLocation());

            // 2. Logica specifica per tipo di unità
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
            {
                // Per il Brawler, premi la vicinanza
                Score += (1000.f - Distance);

                // Verifica se il nemico è attaccabile da questa tile
                if (Distance <= Brawler->GetAttackRange())
                {
                    Score += 500.f; // Bonus per attacco possibile
                }
            }
            else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
            {
                float AttackRange = Sniper->GetAttackRange();

                if (Distance <= AttackRange)
                {
                    Score += 300.f; // Bonus per essere nel range
                }
                else
                {
                    // Penalità se il nemico è troppo lontano
                    Score -= (Distance - AttackRange) * 0.5f;  // Penalità proporzionale alla distanza extra
                }

                // Premiamo la tile se può attaccare più nemici
                int NumEnemiesInRange = 0;
                for (AActor* OtherEnemy : EnemyUnits)
                {
                    if (auto OtherSoldier = Cast<IAW_BaseSoldier>(OtherEnemy))
                    {
                        float DistToOtherEnemy = FVector::Dist(ReachableTile->GetActorLocation(), OtherSoldier->GetTileIsOnNow()->GetActorLocation());
                        if (DistToOtherEnemy <= AttackRange)
                        {
                            NumEnemiesInRange++;
                        }
                    }
                }
                Score += NumEnemiesInRange * 100.f; // Premio per più nemici a portata
            }

            // 3. Penalità per tile pericolose
            if (ReachableTile->GetTileStatus() == ETileStatus::OCCUPIED ||
                ReachableTile->GetTileStatus() == ETileStatus::OBSTACLE)
            {
                Score -= 300.f;
            }

            // 4. Aggiorna la miglior tile
            if (Score > BestScore)
            {
                BestScore = Score;
                BestTile = ReachableTile;
            }
        }
    }

    return BestTile;
}

// Funzione per trovare la tile più vicina al nemico
ATile* AAStarComputerPlayer::FindNearestTileToEnemy(AAW_Sniper* Sniper)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!Sniper || !GameMode || !GameMode->GameField) return nullptr;

    TArray<AActor*> EnemyUnits = GameMode->GetCurrentPlayerUnits(0);
    ATile* BestTile = nullptr;
    float BestPathLength = MAX_FLT;
    ATile* CurrentTile = Sniper->GetTileIsOnNow(); 

    for (AActor* Enemy : EnemyUnits)
    {
        ATile* EnemyTile = nullptr;
        if (AAW_Brawler* EnemyBrawler = Cast<AAW_Brawler>(Enemy))
        {
            EnemyTile = EnemyBrawler->GetTileIsOnNow();
        }
        else if (AAW_Sniper* EnemySniper = Cast<AAW_Sniper>(Enemy))
        {
            EnemyTile = EnemySniper->GetTileIsOnNow();
        }

        if (!EnemyTile) continue;

        TArray<ATile*> FullPath = AStarPathfinding(CurrentTile, EnemyTile);

        if (FullPath.Num() > 0)
        {
            for (int32 i = FMath::Min(FullPath.Num() - 1, Sniper->GetMovementRange()); i >= 0; i--)
            {
                if (Sniper->TilesCanReach.Contains(FullPath[i]))
                {
                    float NewPathLength = AStarPathfinding(FullPath[i], EnemyTile).Num();
                    if (NewPathLength < BestPathLength)
                    {
                        BestPathLength = NewPathLength;
                        BestTile = FullPath[i];
                    }
                    break;
                }
            }
        }
    }

    if (!BestTile) // Fallback
    {
        float MinDirectDistance = MAX_FLT;
        for (AActor* Enemy : EnemyUnits)
        {
            ATile* EnemyTile = nullptr;
            if (auto Soldier = Cast<IAW_BaseSoldier>(Enemy))
            {
                EnemyTile = Soldier->GetTileIsOnNow();
            }

            if (EnemyTile)
            {
                for (ATile* ReachableTile : Sniper->TilesCanReach)
                {
                    float Distance = FVector::Distance(ReachableTile->GetActorLocation(), EnemyTile->GetActorLocation());
                    if (Distance < MinDirectDistance)
                    {
                        MinDirectDistance = Distance;
                        BestTile = ReachableTile;
                    }
                }
            }
        }
    }

    return BestTile;
}







void AAStarComputerPlayer::ExecuteAttack(AActor* AttackingUnit)
{
    if (!AttackingUnit) return;

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;

    TArray<ATile*> AttackableTiles;
    if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(AttackingUnit))
    {
        AttackableTiles = Brawler->GetAttackableTiles();
    }
    else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(AttackingUnit))
    {
        AttackableTiles = Sniper->GetAttackableTiles();
    }

    // Trova il nemico più debole nel range
    AActor* WeakestEnemy = nullptr;
    float LowestHealth = MAX_FLT;

    for (ATile* Tile : AttackableTiles)
    {
        if (AActor* Enemy = Tile->GetUnit())
        {
            float Health = 0.f;
            if (AAW_Brawler* BrawlerEnemy = Cast<AAW_Brawler>(Enemy))
            {
                Health = BrawlerEnemy->GetHealth();
            }
            else if (AAW_Sniper* SniperEnemy = Cast<AAW_Sniper>(Enemy))
            {
                Health = SniperEnemy->GetHealth();
            }
            else {
                continue;
            }
            if (Health < LowestHealth)
            {
                LowestHealth = Health;
                WeakestEnemy = Enemy;
            }
        }
    }

    // Esegui l'attacco
    if (WeakestEnemy)
    {
        ATile* EnemyTile = nullptr;
        if (auto Soldier = Cast<IAW_BaseSoldier>(WeakestEnemy))
        {
            EnemyTile = Soldier->GetTileIsOnNow(); 
        }
        if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(AttackingUnit))
        {
            Brawler->Shoot(EnemyTile);
            bBrawlerAttackedThisTurn = true;
        }
        else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(AttackingUnit))
        {
            Sniper->Shoot(EnemyTile);
            bSniperAttackedThisTurn = true;
        }
    }
    else
    {
        // Nessun nemico nel range, segna come completato
        if (Cast<AAW_Brawler>(AttackingUnit))
        {
            bBrawlerAttackedThisTurn = true;
        }
        else if (Cast<AAW_Sniper>(AttackingUnit))
        {
            bSniperAttackedThisTurn = true;
        }
    }
}





float AAStarComputerPlayer::HeuristicCost(ATile* A, ATile* B)
{
    if (!A || !B) return MAX_FLT; // Sicurezza

    // Calcola la distanza di Manhattan (ottimale per grid 4-direzioni)
    FVector2D Diff = FVector2D(
        FMath::Abs(A->GetGridPosition().X - B->GetGridPosition().X),
        FMath::Abs(A->GetGridPosition().Y - B->GetGridPosition().Y)
    );

    float Distance = Diff.X + Diff.Y;

    // Aggiungi penalità per tile pericolose (es. vicine a nemici)
    if (AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode()))
    {
        if (GameMode->GameField)
        {
            // Penalità base per tile occupate da ostacoli
            if (A->GetTileStatus() == ETileStatus::OCCUPIED || A->GetTileStatus() == ETileStatus::OBSTACLE)
            {
                Distance += 50.0f; // Penalità alta per ostacoli
            }

            // Penalità dinamica per vicinanza a nemici (solo per unità a corto raggio)
            if (SelectedBrawler) // Se stiamo muovendo un Brawler
            {
                TArray<AActor*> EnemyUnits = GameMode->GetCurrentPlayerUnits(1 - PlayerId);
                for (AActor* Enemy : EnemyUnits)
                {
                    if (ATile* EnemyTile = Cast<ATile>(Enemy->GetOwner()))
                    {
                        float EnemyDistance = FVector2D::Distance(
                            A->GetGridPosition(),
                            EnemyTile->GetGridPosition()
                        );
                        if (EnemyDistance < 3) // Se il nemico è entro 3 tile
                        {
                            Distance += 20.0f * (3 - EnemyDistance); // Penalità scalare
                        }
                    }
                }
            }
        }
    }

    return Distance;
}