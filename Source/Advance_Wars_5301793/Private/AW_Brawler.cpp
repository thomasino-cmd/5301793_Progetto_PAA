// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Brawler.h"
#include "Tile.h"
#include "AWGameMode.h"
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
    MoveSpeed = 100.0f;            // Example speed
    

    TileIsOnNow = nullptr;
    OwnerPlayerId = -1;
    CurrentPathIndex = 0;
    MovingCurrentTile = nullptr;


    // Scene component for the tile
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    RootComponent = Scene; // Imposta Scene come RootComponent



    // Static mesh component for the tile's visual representation
    BrawlerStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
    BrawlerStaticMeshComponent->SetupAttachment(RootComponent);
    bIsMoving = false;

  
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

    if (bIsMoving)
    {
        if (CurrentPathIndex < MovementPath.Num())
        {
            ATile* NextTile = MovementPath[CurrentPathIndex];
            FVector TargetLocation = NextTile->GetActorLocation() + FVector(0.0f, 0.0f, 15.0f);

            // Calcola la direzione e la distanza.
            FVector Direction = (TargetLocation - this->GetActorLocation()).GetSafeNormal();
            float DistanceToTarget = FVector::Distance(this->GetActorLocation(), TargetLocation);

            // Calcola quanto muoversi in questo frame.
            float FrameMovement = MoveSpeed * DeltaTime;

;

            UE_LOG(LogTemp, Log, TEXT("Current Location: %s, Target Location: %s, Direction: %s, Distance: %f, FrameMovement: %f"),
                *this->GetActorLocation().ToString(), *TargetLocation.ToString(), *Direction.ToString(), DistanceToTarget, FrameMovement);

            // Se siamo abbastanza vicini, spostati direttamente e passa alla prossima tile.
            if (FrameMovement >= DistanceToTarget) // Using tolerance
            {
                this->SetActorLocation(TargetLocation);
                MovingCurrentTile->SetTileStatus(-1, ETileStatus::EMPTY);
                NextTile->SetTileStatus(OwnerPlayerId, ETileStatus::OCCUPIED);
                UE_LOG(LogTemp, Log, TEXT("Moved unit to tile: %s"), *NextTile->GetName());

                this->SetTileIsOnNow(NextTile);

                MovingCurrentTile = NextTile;
                CurrentPathIndex++;
                UE_LOG(LogTemp, Log, TEXT("Moving to next tile in path.  Index: %d"), CurrentPathIndex);
            }
            else // Altrimenti, muoviti di un passo.
            {
                this->SetActorLocation(this->GetActorLocation() + Direction * FrameMovement);
            }
        }
        else //Movimento terminato
        {
            if (MovementPath.Num() > 0)
            {
                if (this->GetTileIsOnNow() != MovementPath.Last())
                {
                    if (this->GetTileIsOnNow())
                    {
                        this->GetTileIsOnNow()->SetTileStatus(-1, ETileStatus::EMPTY);
                    }
                    MovementPath.Last()->SetTileStatus(OwnerPlayerId, ETileStatus::OCCUPIED);
                    this->SetTileIsOnNow(MovementPath.Last());
                    UE_LOG(LogTemp, Log, TEXT("Movement completed. Final tile occupied."));
                }
            }

            UE_LOG(LogTemp, Log, TEXT("Movement completed."));

           
            this->SetActorRotation(FRotator::ZeroRotator);

            MovingCurrentTile = nullptr;
            MovingTargetTile = nullptr; 
            MovementPath.Empty();
            bIsMoving = false;
            //OnMoveCompleted0.Broadcast(); // Trigger the event        
        }
    }
}













void AAW_Brawler::Attack()
{
   
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



int32 AAW_Brawler::GetAttackRange() const
{
    return AttackRange;
}


struct FTileNode
{
    ATile* Tile;
    int32 Distance;
};





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



void AAW_Brawler::MoveUnit(ATile* TargetTile)
{

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    MovementPath = GameMode->GameField->FindPath(TileIsOnNow, TargetTile);

    if (MovementPath.Num() <= 0)
    {
        return;
    }

    //Inizializza le variabili per il movimento
    MovingCurrentTile = TileIsOnNow;
    MovingTargetTile = TargetTile; //salviamo il target per usarlo alla fine
    MoveSpeed = 200.0f;
    bIsMoving = true; // Inizia il movimento
    CurrentPathIndex = 1; // Inizia dalla prima tile del percorso (dopo quella di partenza)

    
}