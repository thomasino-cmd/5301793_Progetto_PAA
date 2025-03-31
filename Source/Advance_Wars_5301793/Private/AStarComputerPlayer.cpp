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
    // Implementazione base A* (semplificata)
    TMap<ATile*, ATile*> CameFrom;
    TMap<ATile*, float> CostSoFar;
    TArray<ATile*> OpenSet;

    OpenSet.Add(StartTile);
    CameFrom.Add(StartTile, nullptr);
    CostSoFar.Add(StartTile, 0);

    while (OpenSet.Num() > 0)
    {
        ATile* Current = OpenSet[0];
        // Logica completa A* qui...
        // (Implementazione completa richiederebbe priority queue e calcolo costi)
    }

    return Path;
}

void AAStarComputerPlayer::MoveBrawler()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!SelectedBrawler) return;

    SelectedBrawler->TilesCanReach = SelectedBrawler->GetReachableTiles(SelectedBrawler->GetMovementRange());
    GameMode->GameField->HighlightReachableTiles(SelectedBrawler->TilesCanReach);

    TArray<ATile*> ReachableTiles = SelectedBrawler->TilesCanReach;
    ATile* TargetTile = FindOptimalTargetTile(SelectedBrawler, ReachableTiles);

    if (TargetTile)
    {
        SelectedBrawler->MoveUnit(TargetTile);
        // ... resto uguale alla classe base
    }
    // ... stessa logica timer della classe base
}

void AAStarComputerPlayer::MoveSniper()
{
    // Implementazione simile a MoveBrawler con logica A*
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