// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Brawler.h"
#include "Tile.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"




// Sets default values
AAW_Brawler::AAW_Brawler()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    Health = 40.0f;
    MovementRange = 6;
    AttackRange = 1;
    MeleeAttackDamage = FIntPoint(1, 6); // Danno da 1 a 6
    MovementSpeed = 100.0f;            // Example speed

    // Scene component for the tile
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    RootComponent = Scene; // Imposta Scene come RootComponent



    // Static mesh component for the tile's visual representation
    BrawlerStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    BrawlerStaticMeshComponent->SetupAttachment(RootComponent);

  
}

// Called when the game starts or when spawned
void AAW_Brawler::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AAW_Brawler::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Implement interface functions
void AAW_Brawler::Move(FVector Direction)
{
    // Implement movement logic for brawler
    FVector NewLocation = GetActorLocation() + Direction * MovementSpeed * GetWorld()->GetDeltaSeconds();
    SetActorLocation(NewLocation);
}

void AAW_Brawler::Attack(AActor* Target)
{
    // Implement attack logic for brawler
    if (Target && Target->Implements<UAW_BaseSoldier>())
    {
        // Calculate damage
        float Damage = FMath::RandRange(MeleeAttackDamage.X, MeleeAttackDamage.Y);
        IAW_BaseSoldier* SoldierInterface = Cast<IAW_BaseSoldier>(Target);
        SoldierInterface->TakeDamage(Damage);
    }
}

void AAW_Brawler::TakeDamage(float Damage)
{
    Health -= Damage;
    if (Health <= 0)
    {
        // Handle death (e.g., destroy actor)
        Destroy();
    }
}

float AAW_Brawler::GetHealth() const
{
    return Health;
}

int32 AAW_Brawler::GetMovementRange() const
{
    return MovementRange;
}


TArray<FVector2D> AAW_Brawler::GetLegalMoves() const
{
    TArray<FVector2D> LegalMoves;

    //// Ottieni la posizione corrente dalla tile genitore
    //ATile* CurrentTile = Cast<ATile>(GetParentActor());
    //if (!CurrentTile)
    //{
    //    UE_LOG(LogTemp, Error, TEXT("il soldato esiste ma non è impararentato con nessuna tile"))
    //    return LegalMoves;      //TODO se lo ritorna cosi com'è devi assicurarti che parta vuoto tipo un ciclo di inizializzazione a nullptr 
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

int32 AAW_Brawler::GetAttackRange() const
{
    return AttackRange;
}


struct FTileNode
{
    ATile* Tile;
    int32 Distance;
};



//GETREACHEABLETILES funziona ma mi va a modificare il riferimento alla tile su cui è ora quindi incasina moveunit 

//TArray<ATile*> AAW_Brawler::GetReachableTiles(int32 Range)
//{
//    TArray<ATile*> ReachableTiles;
//    AGameField* GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
//    if (!GameField)
//    {
//        return ReachableTiles; // Or handle the error appropriately
//    }
//
//    ATile* StartTile = TileIsOnNow;  // Use the stored reference
//    if (!StartTile)
//    {
//        return ReachableTiles;
//    }
//
//    // 1. Initialization
//    TQueue<FTileNode> TileQueue;
//    TMap<ATile*, ATile*> CameFrom; // To reconstruct the path (if needed later)
//    TMap<ATile*, int32> Distance;
//
//    FTileNode StartNode;
//    StartNode.Tile = StartTile;
//    StartNode.Distance = 0;
//
//    TileQueue.Enqueue(StartNode);
//    Distance.Add(StartTile, 0);
//    ReachableTiles.Add(StartTile);
//
//    // 2. BFS Algorithm
//    while (!TileQueue.IsEmpty())
//    {
//        FTileNode CurrentNode;
//        TileQueue.Dequeue(CurrentNode);
//        TileIsOnNow = CurrentNode.Tile;
//        int32 CurrentDistance = CurrentNode.Distance;
//
//        // 3. Explore Neighbors
//        TArray<ATile*> Neighbors;
//        FVector2D CurrentPosition = TileIsOnNow->GetGridPosition();
//
//        // Define possible neighbor offsets
//        TArray<FVector2D> Directions = {
//            FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1)
//        };
//
//        for (const FVector2D& Dir : Directions)
//        {
//            FVector2D NeighborPosition = CurrentPosition + Dir;
//            ATile* NeighborTile = GameField->GetTile(NeighborPosition.X, NeighborPosition.Y);
//            if (NeighborTile)
//            {
//                Neighbors.Add(NeighborTile);
//            }
//        }
//
//        for (ATile* NeighborTile : Neighbors)
//        {
//            // 4. Check Validity and Distance
//            if (NeighborTile->GetTileStatus() != ETileStatus::OBSTACLE )
//            {
//                if (!Distance.Contains(NeighborTile) && CurrentDistance + 1 <= Range)
//                {
//                    // 5. Enqueue and Update
//                    FTileNode NextNode;
//                    NextNode.Tile = NeighborTile;
//                    NextNode.Distance = CurrentDistance + 1;
//
//                    TileQueue.Enqueue(NextNode);
//                    Distance.Add(NeighborTile, CurrentDistance + 1);
//                    CameFrom.Add(NeighborTile, TileIsOnNow); // For path reconstruction
//                    ReachableTiles.Add(NeighborTile);
//                }
//            }
//        }
//    }
//    return ReachableTiles;
//}


TArray<ATile*> AAW_Brawler::GetReachableTiles(int32 Range)
{
    TArray<ATile*> ReachableTiles;
    AGameField* GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!GameField)
    {
        return ReachableTiles; // Or handle the error appropriately
    }

    ATile* StartTile = TileIsOnNow;  // Use the stored reference
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
        ATile* CurrentTile = CurrentNode.Tile;  // Usare una variabile locale
        int32 CurrentDistance = CurrentNode.Distance;

        // 3. Explore Neighbors
        TArray<ATile*> Neighbors;
        FVector2D CurrentPosition = CurrentTile->GetGridPosition(); // Usare CurrentTile

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
                    CameFrom.Add(NeighborTile, CurrentTile); // Per il path reconstruction
                    ReachableTiles.Add(NeighborTile);
                }
            }
        }
    }
    return ReachableTiles;
}

ATile* AAW_Brawler::GetTileIsOnNow() const
{
    return TileIsOnNow;
}

void AAW_Brawler::SetTileIsOnNow(ATile* NewTile)
{
    TileIsOnNow = NewTile;
}
