// Fill out your copyright notice in the Description page of Project Settings.

#include "GameField.h"
#include "DrawDebugHelpers.h"  // For drawing debug lines
#include "Kismet/GameplayStatics.h"

// Sets default values
AGameField::AGameField()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = false;

    // Initialize default values for the size, tile size, and tile spacing
    Size = 25;
    TileSize = 100.0f;
    TileSpacing = 10.0f;
    CellPadding = 0.2f;
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
            TileMap.Add(FVector2D(IndexX, IndexY), Obj);
        }
    }
}

// Get the relative location of a tile based on its grid position
FVector AGameField::GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const
{
    // Calculate the relative location of the tile based on its grid position, tile size, and the multiplier
    return TileSize * NextCellPositionMultiplier * FVector(InX, InY, 0);
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



