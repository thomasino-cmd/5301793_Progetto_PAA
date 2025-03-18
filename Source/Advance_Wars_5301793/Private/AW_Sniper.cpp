// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Sniper.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AAW_Sniper::AAW_Sniper()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Health = 20.0f;
    MovementRange = 3;
    AttackRange = 10;
    RangedAttackDamage = FIntPoint(4, 8); // Danno da 4 a 8
    MovementSpeed = 100.0f;            // Example speed

    // Scene component for the tile
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    RootComponent = Scene; // Imposta Scene come RootComponent



    // Static mesh component for the tile's visual representation
    BrawlerStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    BrawlerStaticMeshComponent->SetupAttachment(RootComponent);

}

// Called when the game starts or when spawned
void AAW_Sniper::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AAW_Sniper::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Implement interface functions
void AAW_Sniper::Move(FVector Direction)
{
    // Implement movement logic for sniper
    FVector NewLocation = GetActorLocation() + Direction * MovementSpeed * GetWorld()->GetDeltaSeconds();
    SetActorLocation(NewLocation);
}

void AAW_Sniper::Attack(AActor* Target)
{
    // Implement attack logic for sniper
    if (Target && Target->Implements<UAW_BaseSoldier>())
    {
        // Calculate damage
        float Damage = FMath::RandRange(RangedAttackDamage.X, RangedAttackDamage.Y);
        IAW_BaseSoldier* SoldierInterface = Cast<IAW_BaseSoldier>(Target);
        SoldierInterface->TakeDamage(Damage);
    }
}

void AAW_Sniper::TakeDamage(float Damage)
{
    Health -= Damage;
    if (Health <= 0)
    {
        // Handle death (e.g., destroy actor)
        Destroy();
    }
}

float AAW_Sniper::GetHealth() const
{
    return Health;
}



struct FTileNode
{
    ATile* Tile;
    int32 Distance;
};

TArray<ATile*> AAW_Sniper::GetReachableTiles(int32 Range)
{
    TArray<ATile*> ReachableTiles;
    AGameField* GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!GameField)
    {
        return ReachableTiles; // Or handle the error appropriately
    }
    ATile* StartTile = CurrentTile;  // Use the stored reference
    if (!StartTile)
    {
        return ReachableTiles;
    }
    // 1. Initialization
    TQueue<FTileNode> TileQueue;
    TMap<ATile*, ATile*> CameFrom; // To reconstruct the path (if needed later)
    TMap<ATile*, int32> Distance;

    FTileNode StartNode;
    StartNode.Tile = StartTile;
    StartNode.Distance = 0;

    TileQueue.Enqueue(StartNode);
    Distance.Add(StartTile, 0);
    ReachableTiles.Add(StartTile);

    // 2. BFS Algorithm
    while (!TileQueue.IsEmpty())
    {
        FTileNode CurrentNode;
        TileQueue.Dequeue(CurrentNode);
        CurrentTile = CurrentNode.Tile;
        int32 CurrentDistance = CurrentNode.Distance;

        // 3. Explore Neighbors
        TArray<ATile*> Neighbors;
        FVector2D CurrentPosition = CurrentTile->GetGridPosition();

        // Define possible neighbor offsets
        TArray<FVector2D> Directions = {
            FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1)
        };

        for (const FVector2D& Dir : Directions)
        {
            FVector2D NeighborPosition = CurrentPosition + Dir;
            ATile* NeighborTile = GameField->GetTile(NeighborPosition.X, NeighborPosition.Y);
            if (NeighborTile)
            {
                Neighbors.Add(NeighborTile);
            }
        }

        for (ATile* NeighborTile : Neighbors)
        {
            // 4. Check Validity and Distance
            if (NeighborTile->GetTileStatus() != ETileStatus::OBSTACLE)
            {
                if (!Distance.Contains(NeighborTile) && CurrentDistance + 1 <= Range)
                {
                    // 5. Enqueue and Update
                    FTileNode NextNode;
                    NextNode.Tile = NeighborTile;
                    NextNode.Distance = CurrentDistance + 1;

                    TileQueue.Enqueue(NextNode);
                    Distance.Add(NeighborTile, CurrentDistance + 1);
                    CameFrom.Add(NeighborTile, CurrentTile); // For path reconstruction
                    ReachableTiles.Add(NeighborTile);
                }
            }
        }
    }
    return ReachableTiles;
}

int32 AAW_Sniper::GetMovementRange() const
{
    return MovementRange;
}

int32 AAW_Sniper::GetAttackRange() const
{
    return AttackRange;
}

// Function to calculate counter-attack damage
float AAW_Sniper::CalculateCounterAttackDamage(AActor* Attacker)
{
    // Simplified logic (no need to check attacker type)
    return FMath::RandRange(1, 3);
}



TArray<FVector2D> AAW_Sniper::GetLegalMoves() const
{
    TArray<FVector2D> LegalMoves;

    //// Ottieni la posizione corrente dalla tile genitore
    //ATile* CurrentTile = Cast<ATile>(GetParentActor());
    //if (!CurrentTile)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("il soldato esiste ma non è impararentato con nessuna tile"))
    //        return LegalMoves;      //TODO se lo ritorna cosi com'è devi assicurarti che parta vuoto tipo un ciclo di inizializzazione a nullptr 
    //}
    //FVector2D CurrentPosition = CurrentTile->GetGridPosition();

    //// Ottieni un riferimento al GameField
    //AGameField* Field = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    //if (!Field)
    //{
    //    return LegalMoves;
    //}

    //int32 Range = GetMovementRange();
    //int32 FieldSize = Field->GetSize();

    //for (int32 X = 0; X < FieldSize; ++X)
    //{
    //    for (int32 Y = 0; Y < FieldSize; ++Y)
    //    {
    //        FVector2D TargetPosition(X, Y);
    //        float Distance = FVector2D::Distance(CurrentPosition, TargetPosition);      //IMPORTANTE TODO non sono sicuro che distanza in linea d'aria sia corretta come metrica, non deve fare una sfera . ma questo è cambiabile .

    //        if (Distance <= Range)
    //        {
    //            ATile* TargetTile = Field->GetTile(X, Y);
    //            if (TargetTile && TargetTile->GetTileStatus() != ETileStatus::OBSTACLE && TargetTile->GetUnit() == nullptr)
    //            {
    //                LegalMoves.Add(TargetPosition);
    //            }
    //        }
    //    }
    //}
    return LegalMoves;
}
