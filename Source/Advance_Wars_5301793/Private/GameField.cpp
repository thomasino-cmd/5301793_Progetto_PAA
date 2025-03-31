// Fill out your copyright notice in the Description page of Project Settings.

#include "GameField.h"
#include "DrawDebugHelpers.h"  // For drawing debug lines
#include "Kismet/GameplayStatics.h"
#include "AWGameMode.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Materials/Material.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"
#include "AWGameInstance.h"

// Sets default values
AGameField::AGameField()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // Initialize default values for the size, tile size, and tile spacing
    Size = 25;
    TileSize = 100.0f;
    TileSpacing = 0.01f;
    CellPadding = 0.02f;
}


void AGameField::OnConstruction(const FTransform& Transform)
{
    Super::OnConstruction(Transform);

    // Calculate the NextCellPositionMultiplier based on TileSize and CellPadding
    NextCellPositionMultiplier = (TileSize + TileSize * CellPadding) / TileSize;
  
}

// Called when the game starts or when spawned
void AGameField::BeginPlay()
{
    Super::BeginPlay();

    // Generate the game field when the game starts
    GenerateField();
}

void AGameField::GenerateField()
{
    // Clear any existing tiles
    TileMap.Empty();

    // Create the grid of tiles
    for (int32 IndexX = 0; IndexX < Size; IndexX++)
    {
        for (int32 IndexY = 0; IndexY < Size; IndexY++)
        {
            FVector Location = AGameField::GetRelativeLocationByXYPosition(IndexX, IndexY);
            ATile* Obj = GetWorld()->SpawnActor<ATile>(TileClass, Location, FRotator::ZeroRotator);
            const float TileScale = TileSize / 100.f;
            const float Zscaling = 0.2f;
            Obj->SetActorScale3D(FVector(TileScale, TileScale, Zscaling));
            Obj->SetGridPosition(IndexX, IndexY);
            // Add the tile to the TileMap
            
            Obj->SetTileStatus(NOT_ASSIGNED, ETileStatus::EMPTY);

            TileMap.Add(FVector2D(IndexX, IndexY), Obj);
            
        }
    }
    // Log message for field generation
    FString message = "Game Field Generated!";
    if (UAWGameInstance* GameInstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())))
    {
        GameInstance->SetTurnMessage(message);
    }
    UE_LOG(LogTemp, Warning, TEXT("Game Field Generated!"));




    GetWorld()->GetTimerManager().SetTimer(ObstacleSpawnTimerHandle, this, &AGameField::CheckCoinFlipStatus, 0.5f, true);

}

void AGameField::CheckCoinFlipStatus()
{
    if (AAWGameMode* GameMode = Cast<AAWGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
    {
        if (GameMode->bCoinFlipCompleted)
        {
            SpawnRandomObstacles();
            GetWorld()->GetTimerManager().ClearTimer(ObstacleSpawnTimerHandle);
        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Waiting for coin flip to complete..."));
        }
    }
}


void AGameField::SpawnRandomObstacles()
{
    int32 totalTiles = Size * Size;
    int32 targetObstacleCount = FMath::RoundToInt(totalTiles * 0.20f); // 10%
    int32 obstacleCount = 0;
    int32 maxAttempts = 1000; // Limite per evitare loop infiniti

    while (obstacleCount < targetObstacleCount && maxAttempts-- > 0)
    {
        int32 x = FMath::RandRange(0, Size - 1);
        int32 y = FMath::RandRange(0, Size - 1);
        ATile* tile = GetTile(x, y);

        if (tile && tile->GetTileStatus() == ETileStatus::EMPTY)
        {
            // Simula temporaneamente l'ostacolo
            tile->SetTileStatus(-2, ETileStatus::OBSTACLE);

            // Verifica se il campo rimane accessibile
            if (IsGameFieldAccessible())
            {
                // Se sì, spawna l'ostacolo definitivamente
                TSubclassOf<AObstacle> obstacleClass = MountainClass;
                float randomValue = FMath::FRand();
                if (randomValue < 0.4f) obstacleClass = Tree1Class;
                else if (randomValue < 0.7f) obstacleClass = Tree2Class;

                FVector spawnLocation = tile->GetActorLocation();
                AObstacle* obstacle = GetWorld()->SpawnActor<AObstacle>(obstacleClass, spawnLocation, FRotator::ZeroRotator);

                if (obstacle)
                {
                    obstacle->AttachToActor(tile, FAttachmentTransformRules::KeepWorldTransform);
                    obstacleCount++;
                }
            }
            else
            {
                // Altrimenti, annulla il cambiamento
                tile->SetTileStatus(0, ETileStatus::EMPTY);
            }
        }
    }
}

bool AGameField::IsGameFieldAccessible()
{
    // Usa BFS per verificare che tutti i tile vuoti siano raggiungibili
    TArray<FIntPoint> emptyTiles;

    // Trova tutti i tile vuoti
    for (int32 y = 0; y < Size; ++y)
    {
        for (int32 x = 0; x < Size; ++x)
        {
            ATile* tile = GetTile(x, y);
            if (tile && tile->GetTileStatus() == ETileStatus::EMPTY)
            {
                emptyTiles.Add(FIntPoint(x, y));
            }
        }
    }

    if (emptyTiles.Num() == 0) return true; // Nessun tile vuoto

    // Esegui BFS a partire dal primo tile vuoto
    TSet<FIntPoint> visited;
    TQueue<FIntPoint> queue;
    queue.Enqueue(emptyTiles[0]);
    visited.Add(emptyTiles[0]);

    int32 connectedCount = 0;
    const int32 Directions[4][2] = { {-1, 0}, {1, 0}, {0, -1}, {0, 1} };

    while (!queue.IsEmpty())
    {
        FIntPoint current;
        queue.Dequeue(current);
        connectedCount++;

        for (const auto& dir : Directions)
        {
            int32 nx = current.X + dir[0];
            int32 ny = current.Y + dir[1];
            FIntPoint neighbor(nx, ny);

            if (nx >= 0 && nx < Size && ny >= 0 && ny < Size &&
                !visited.Contains(neighbor))
            {
                ATile* tile = GetTile(nx, ny);
                if (tile && tile->GetTileStatus() == ETileStatus::EMPTY)
                {
                    visited.Add(neighbor);
                    queue.Enqueue(neighbor);
                }
            }
        }
    }

    // Se tutti i tile vuoti sono connessi, ritorna true
    return (connectedCount == emptyTiles.Num());
}


FVector2D AGameField::GetXYPositionByRelativeLocation(const FVector& Location) const
{
    const double x = Location[0] / (TileSize * NextCellPositionMultiplier);
    const double y = Location[1] / (TileSize * NextCellPositionMultiplier);
    return FVector2D(x, y);
}

// Get the relative location of a tile based on its grid position
FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
    // Calculate the relative location of the tile based on its grid position, tile size, and the multiplier
    return (TileSize * NextCellPositionMultiplier) * FVector(InX, InY, 0);
}


void AGameField::ResetField()
{
    // Reset all tiles
    for (ATile* Tile : TileArray)
    {
        Tile->SetTileStatus(NOT_ASSIGNED, ETileStatus::EMPTY);

        // Distruggi qualsiasi unità o ostacolo sulla tile
        if (AActor* Unit = Tile->GetUnit())
        {
            Unit->Destroy();
        }
    }

    // Distruggi tutti gli ostacoli rimanenti (se non associati a tile)
    TArray<AActor*> Obstacles;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), AObstacle::StaticClass(), Obstacles);
    for (AActor* Obstacle : Obstacles)
    {
        Obstacle->Destroy();
    }

    // Distruggi tutte le unità rimanenti
    TArray<AActor*> Soldiers;
    UGameplayStatics::GetAllActorsWithInterface(GetWorld(), UAW_BaseSoldier::StaticClass(), Soldiers);
    for (AActor* Soldier : Soldiers)
    {
        Soldier->Destroy();
    }

    // Distruggi la moneta se esiste
    if (ACoin* Coin = Cast<ACoin>(UGameplayStatics::GetActorOfClass(GetWorld(), ACoin::StaticClass())))
    {
        Coin->Destroy();
    }

    // Resetta gli stati del GameMode
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->bIsGameOver = false;
        GameMode->MoveCounter = 0;
        GameMode->bIsPlacementPhaseOver = false;
        GameMode->bCoinFlipCompleted = false;
    }

    // Broadcast dell'evento di reset
    OnResetEvent.Broadcast();
}





// Get a tile at the specified grid position
ATile* AGameField::GetTile(int32 X, int32 Y) const
{
    // Check if the given coordinates are within the bounds of the grid
    if (X >= 0 && X < Size && Y >= 0 && Y < Size)
    {
        // Access the tile from the TileMap using FVector2D as the key
        return TileMap[FVector2D(X, Y)];
    }
    return nullptr;
}

// Get the size of the game field (number of tiles in each dimension)
int32 AGameField::GetSize() const
{
    return Size;
}

// Set the size of the game field (number of tiles in each dimension)
void AGameField::SetSize(int32 NewSize)
{
    Size = NewSize;
}

// Get the tile size in world units
float AGameField::GetTileSize() const
{
    return TileSize;
}


void AGameField::SetGridCellOccupied(const FVector2D& GridPosition, int32 PlayerNumber)
{
    ATile** TilePtr = TileMap.Find(GridPosition); // Use Find

    if (TilePtr) // Check if the pointer is valid
    {
        ATile* Tile = *TilePtr; // Dereference to get the ATile

        Tile->SetPlayerOwner(PlayerNumber);
        Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("Invalid grid position: %s"), *GridPosition.ToString());
    }
}

// Set the tile size in world units
void AGameField::SetTileSize(float NewTileSize)
{
    TileSize = NewTileSize;
}

// Get the distance between tiles in world units
float AGameField::GetTileSpacing() const
{
    return TileSpacing;
}

// Set the distance between tiles in world units
void AGameField::SetTileSpacing(float NewTileSpacing)
{
    TileSpacing = NewTileSpacing;
}

void AGameField::ResetGameStatusField()
{
    for (ATile* CurrentTile : TileArray)
    {
        if (CurrentTile->GetTileStatus() != ETileStatus::EMPTY)
        {
            CurrentTile->SetTileStatus(-1, ETileStatus::EMPTY);
        }
    }
}

void AGameField::HighlightReachableTiles(const TArray<ATile*>& ReachableTiles)
{
    for (ATile* Tile : ReachableTiles)
    {
        if (Tile)
        {
            Tile->Highlight(true); // Assuming Tile has a Highlight(bool) function
        }
    }
}

void AGameField::ClearHighlightedTiles(const TArray<ATile*>& ReachableTiles)
{
    for (ATile* Tile : ReachableTiles)
    {
        if (Tile)
        {
            Tile->Highlight(false); // Assuming Tile has a Highlight(bool) function
        }
    }
}

void AGameField::HighlightAttackTiles(const TArray<ATile*>& AttackableTiles, int32 PlayerId)
{
    if (AttackableTiles.Num() == 0) return; // Usciamo se non ci sono tile da evidenziare

    for (ATile* Tile : AttackableTiles)
    {
        if (Tile)
        {
            int32 TilePlayerId = Tile->GetTileOwner(); // Assumiamo che ATile abbia un metodo GetPlayerId()
            ETileStatus TileStat = Tile->GetTileStatus();
            if(TilePlayerId != PlayerId && TilePlayerId != -2 && TilePlayerId != -1)                          //(TilePlayerId != PlayerId && TileStat != ETileStatus::OBSTACLE) // Verifica se la tile è occupata da un altro giocatore
            {
                UStaticMeshComponent* TileMesh = Tile->GetStaticMeshComponent(); // Assumiamo che ATile abbia un componente StaticMesh

                if (TileMesh)
                {
                    UMaterialInterface* BaseMaterial = TileMesh->GetMaterial(0); // Ottieni il materiale base della tile
                    UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(BaseMaterial, nullptr);

                    if (DynamicMaterial)
                    {
                        // Imposta il colore a rosso fosforescente (o altro colore desiderato)
                        DynamicMaterial->SetVectorParameterValue(FName("Color"), FLinearColor::Red);
                        DynamicMaterial->SetScalarParameterValue(FName("EmissiveStrength"), 5.0f); // Aggiungi emissione per l'effetto fosforescente

                        TileMesh->SetMaterial(0, DynamicMaterial); // Applica il materiale dinamico alla tile
                    }
                }
            }
        }
    }
}




void AGameField::ClearHighlightedAttackTiles(const TArray<ATile*>& AttackableTiles)
{
    for (ATile* Tile : AttackableTiles)
    {
        if (Tile)
        {
            UStaticMeshComponent* TileMesh = Tile->GetStaticMeshComponent();
            if (TileMesh)
            {
                //Ottieni il materiale base originale.
                UMaterialInterface* BaseMaterial = TileMesh->GetMaterial(0);
                //Ripristina il materiale originale.
                TileMesh->SetMaterial(0, BaseMaterial);
            }
        }
    }
}


TArray<ATile*> AGameField::GetNeighborTiles(ATile* CurrentTile) const
{
    TArray<ATile*> Neighbors;

    if (!CurrentTile) return Neighbors;

    // Ottieni le coordinate della tile
    FVector2D TilePos = CurrentTile->GetGridPosition();

    int32 X = TilePos.X;
    int32 Y = TilePos.Y;

    // Controlla le 4 direzioni cardinali (senza diagonali)
    ATile* Neighbor;

    // Sopra
    Neighbor = GetTile(X, Y + 1);
    if (Neighbor) Neighbors.Add(Neighbor);

    // Sotto
    Neighbor = GetTile(X, Y - 1);
    if (Neighbor) Neighbors.Add(Neighbor);

    // Sinistra
    Neighbor = GetTile(X - 1, Y);
    if (Neighbor) Neighbors.Add(Neighbor);

    // Destra
    Neighbor = GetTile(X + 1, Y);
    if (Neighbor) Neighbors.Add(Neighbor);

    return Neighbors;

}





// LOGIC FOR PATHFINDING AND MOVEMENT HERE 

TArray<ATile*> AGameField::FindPath(ATile* StartTile, ATile* GoalTile)
{
    TArray<ATile*> Path; // The path to return
    if (!StartTile || !GoalTile)
    {
        UE_LOG(LogTemp, Warning, TEXT("FindPath: Start or Goal tile is null"));
        return Path; // Return an empty path
    }

    TMap<ATile*, ATile*> CameFrom;
    TMap<ATile*, float> GCost;
    TMap<ATile*, float> FCost;
    TSet<ATile*> OpenSet;
    TSet<ATile*> ClosedSet;

    // Initialize
    OpenSet.Add(StartTile);
    GCost.Add(StartTile, 0.0f);
    FCost.Add(StartTile, CalculateHCost(StartTile, GoalTile));

    while (OpenSet.Num() > 0)
    {
        ATile* CurrentTile = GetLowestFCostTile(OpenSet, FCost);
        if (CurrentTile == GoalTile)
        {
            // Reconstruct path
            Path = ReconstructPath(CameFrom, CurrentTile);
            return Path;
        }

        OpenSet.Remove(CurrentTile);
        ClosedSet.Add(CurrentTile);

        TArray<ATile*> Neighbors = GetNeighbors(CurrentTile);
        for (ATile* Neighbor : Neighbors)
        {
            if (ClosedSet.Contains(Neighbor))
            {
                continue;
            }

            float TentativeGCost = GCost[CurrentTile] + 1.0f; // Cost to move to neighbor is 1

            if (!OpenSet.Contains(Neighbor))
            {
                OpenSet.Add(Neighbor);
            }
            else if (TentativeGCost >= GCost[Neighbor])
            {
                continue;
            }

            CameFrom.Add(Neighbor, CurrentTile);
            GCost.Add(Neighbor, TentativeGCost);
            FCost.Add(Neighbor, TentativeGCost + CalculateHCost(Neighbor, GoalTile));
        }
    }

    // No path found
    UE_LOG(LogTemp, Warning, TEXT("FindPath: No path found"));
    return Path; // Return an empty path
}

TArray<ATile*> AGameField::GetNeighbors(ATile* Tile)
{
    TArray<ATile*> Neighbors;
    FVector2D TilePosition = Tile->GetGridPosition();
    int32 X = static_cast<int32>(TilePosition.X);
    int32 Y = static_cast<int32>(TilePosition.Y);

    // Check each direction (no diagonals)
    AddNeighborIfValid(Neighbors, X + 1, Y); // Right
    AddNeighborIfValid(Neighbors, X - 1, Y); // Left
    AddNeighborIfValid(Neighbors, X, Y + 1); // Up
    AddNeighborIfValid(Neighbors, X, Y - 1); // Down

    return Neighbors;
}

void AGameField::AddNeighborIfValid(TArray<ATile*>& Neighbors, int32 X, int32 Y)
{
    ATile* Neighbor = GetTile(X, Y);
    if (Neighbor && Neighbor->GetTileStatus() != ETileStatus::OBSTACLE && Neighbor->GetTileStatus() != ETileStatus::OCCUPIED)
    {
        Neighbors.Add(Neighbor);
    }
}

ATile* AGameField::GetLowestFCostTile(const TSet<ATile*>& OpenSet, const TMap<ATile*, float>& FCost)
{
    ATile* LowestTile = nullptr;
    float LowestCost = MAX_FLT; // Initialize to max float

    for (ATile* Tile : OpenSet)
    {
        if (FCost[Tile] < LowestCost)
        {
            LowestCost = FCost[Tile];
            LowestTile = Tile;
        }
    }
    return LowestTile;
}

float AGameField::CalculateHCost(ATile* Start, ATile* Goal)
{
    FVector2D StartPos = Start->GetGridPosition();
    FVector2D GoalPos = Goal->GetGridPosition();
    return FMath::Abs(StartPos.X - GoalPos.X) + FMath::Abs(StartPos.Y - GoalPos.Y); // Manhattan distance
}

TArray<ATile*> AGameField::ReconstructPath(const TMap<ATile*, ATile*>& CameFrom, ATile* Current)
{
    TArray<ATile*> Path;
    while (Current)
    {
        Path.Insert(Current, 0); // Insert at the beginning to reverse the path
        Current = CameFrom.Contains(Current) ? CameFrom[Current] : nullptr;
    }
    return Path;
}

