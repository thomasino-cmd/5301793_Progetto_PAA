// Fill out your copyright notice in the Description page of Project Settings.

#include "GameField.h"
#include "DrawDebugHelpers.h"  // For drawing debug lines
#include "Kismet/GameplayStatics.h"
#include "AWGameMode.h"

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

    // Set Timer to call SpawnRandomObstacles after a delay
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
        {
            SpawnRandomObstacles();
        }, 1.0f, false);
}



#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "AWGameInstance.h"

void AGameField::SpawnRandomObstacles()
{
    // 1. Calculate the number of obstacles to spawn
    int32 totalTiles = Size * Size;
    int32 targetObstacleCount = FMath::RoundToInt(totalTiles * 0.10f); // 10%

    int32 obstacleCount = 0;

   
   
    // 2. Spawn obstacles until the target is reached
    
    while (obstacleCount < targetObstacleCount)
    {
        
       
                
                int32 x = FMath::RandRange(0, Size - 1);
                int32 y = FMath::RandRange(0, Size - 1);

                ATile* tile = GetTile(x, y);

                // 3. Check if the tile is valid and empty
                if (tile && tile->GetTileStatus() == ETileStatus::EMPTY)
                {
                    // 4. Determine the obstacle class using weighted randomization
                    TSubclassOf<AObstacle> obstacleClass = MountainClass; // Default to mountain

                    float randomValue = FMath::FRand();
                    if (randomValue < 0.4f) // 40% chance for Tree1
                    {
                        obstacleClass = Tree1Class;
                    }
                    else if (randomValue < 0.7f) // 30% chance for Tree2
                    {
                        obstacleClass = Tree2Class;
                    }
                    // else 30% Mountain

                    // 5. Spawn the obstacle
                    FVector spawnLocation = tile->GetActorLocation();
                    AObstacle* obstacle = GetWorld()->SpawnActor<AObstacle>(obstacleClass, spawnLocation, FRotator::ZeroRotator);

                    if (obstacle)
                    {
                        //Parent the obstacle to the tile
                        obstacle->AttachToActor(tile, FAttachmentTransformRules::KeepWorldTransform);

                        // 6. Mark the tile as occupied
                        tile->SetTileStatus(-2, ETileStatus::OBSTACLE); // Or use a specific value to indicate an obstacle
                        obstacleCount++;

                        
                    }
                }
                

    }
    
}






// Get the relative location of a tile based on its grid position
FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
    // Calculate the relative location of the tile based on its grid position, tile size, and the multiplier
    return (TileSize * NextCellPositionMultiplier) * FVector(InX, InY, 0);
}

void AGameField::ResetField()
{
    for (ATile* Obj : TileArray)
    {
        Obj->SetTileStatus(NOT_ASSIGNED, ETileStatus::EMPTY);
    }

    // send broadcast event to registered objects 
    OnResetEvent.Broadcast();

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    GameMode->bIsGameOver = false;
    GameMode->MoveCounter = 0;
    GameMode->ChoosePlayerAndStartGame();
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
    if (TileMap.Contains(GridPosition))
    {
        ATile* Tile = TileMap[GridPosition];

        Tile->SetPlayerOwner(PlayerNumber);
        Tile->SetTileStatus(PlayerNumber, ETileStatus::OCCUPIED);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("invalid grid position: %s"), *GridPosition.ToString());
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

    //PROBABLY NOT NECESSARY  SetSelectedTile(FVector2D(-1, -1));
}


void AGameField::SetLegalMoves(const TArray<FVector2D>& NewLegalMoves)
{
    LegalMovesArray = NewLegalMoves;
}


void AGameField::ShowLegalMovesInTheField()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode is null in ShowLegalMovesInTheField!"));
        return; // Exit if GameMode is invalid
    }

    for (const FVector2D& Position : LegalMovesArray)
    {
        ATile* CurrentTile = TileMap.FindRef(Position);
        if (CurrentTile) // Check if the tile is valid
        {
            ////in realt� io dovrei potermi muovere solo su tiles che sono empty quindi ancora inizializzate a -1. 
            //if (CurrentTile->GetTileOwner() != GameMode->CurrentPlayer &&  CurrentTile->GetTileOwner() != -1)
            //{
            //    //CurrentTile->SetTileStatus(CurrentTile->GetTileOwner(), ETileStatus::OBSTACLE); // Or a new status like CAN_ATTACK
            //    UE_LOG(LogTemp, Warning, TEXT("Must have clicked on an obstacle"))
            //}
            //else
            //{
            //    CurrentTile->SetTileStatus(CurrentTile->GetTileOwner(), ETileStatus::OCCUPIED);
            //}

            if (CurrentTile->GetTileStatus() != ETileStatus::OCCUPIED && CurrentTile->GetTileStatus() != ETileStatus::OBSTACLE) 
            {
                CurrentTile->SetTileMaterial();
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                           
            }  


        }
        else
        {
            UE_LOG(LogTemp, Warning, TEXT("Tile not found at position: %s"), *Position.ToString());
        }
    }
}

