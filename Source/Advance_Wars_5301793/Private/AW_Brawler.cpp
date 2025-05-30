// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Brawler.h"
#include "Tile.h"
#include "AWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/BoxComponent.h"
#include "AWPlayerController.h"




// Sets default values
AAW_Brawler::AAW_Brawler()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;
    StartingHealth = 40.0f;
    Health = StartingHealth;
    MovementRange = 6;
    AttackRange = 1;
    MeleeAttackDamage = FIntPoint(1, 6); // Danno da 1 a 6
    MoveSpeed = 150.0f;          

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
        }
    }
}

void AAW_Brawler::Attack()
{
   
}

void AAW_Brawler::TakeDamage(float Damage)
{
    Health -= Damage;
    FString DamageString = FString::Printf(TEXT("%f"), Damage);
   // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Subiti %s danni :/ "), *DamageString));

   AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

   if (Health <= 0)
    {
       // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("vita esaurita. morte "));

        if (GameMode)
        {
            // Remove from appropriate array
            if (OwnerPlayerId == 0)
            {
                GameMode->Player1Brawlers.Remove(this);
            }
            else if (OwnerPlayerId == 1)
            {
                GameMode->Player2Brawlers.Remove(this);
            }

            // Check win condition immediately when a unit dies
            GameMode->CheckWinCondition();
        }

        ATile* Tile = this->GetTileIsOnNow();
        if (Tile)
        {
            Tile->SetTileStatus(-1, ETileStatus::EMPTY);
        }
        Destroy();
    }
    GameMode->UpdateHUD();
}

float AAW_Brawler::GetHealth() const
{
    return Health;
}

int32 AAW_Brawler::GetMovementRange() const
{
    return MovementRange;
}

int32 AAW_Brawler::GetOwnerPlayerId() const
{
    return OwnerPlayerId;
}

int32 AAW_Brawler::GetAttackRange() const
{
    return AttackRange;
}

TArray<ATile*> AAW_Brawler::GetAttackableTiles()
{
    TArray<ATile*> AttackableTiles;
    AGameField* GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!GameField)
    {
        return AttackableTiles;
    }

    ATile* StartTile = TileIsOnNow;
    if (!StartTile)
    {
        return AttackableTiles;
    }

    int32 Range = GetAttackRange();

    // 1. Initialization
    TQueue<FTileNode> TileQueue;
    TMap<ATile*, int32> Distance;

    FTileNode StartNode;
    StartNode.Tile = StartTile;
    StartNode.Distance = 0;

    TileQueue.Enqueue(StartNode);
    Distance.Add(StartTile, 0);

    // 2. BFS Algorithm
    while (!TileQueue.IsEmpty())
    {
        FTileNode CurrentNode;
        TileQueue.Dequeue(CurrentNode);
        ATile* CurrentTile = CurrentNode.Tile;
        int32 CurrentDistance = CurrentNode.Distance;

        // 3. Explore Neighbors
        TArray<FVector2D> Directions = {
            FVector2D(1, 0), FVector2D(-1, 0), FVector2D(0, 1), FVector2D(0, -1)
        };

        for (const FVector2D& Dir : Directions)
        {
            FVector2D NeighborPosition = CurrentTile->GetGridPosition() + Dir;
            ATile* NeighborTile = GameField->GetTile(NeighborPosition.X, NeighborPosition.Y);

            if (NeighborTile && !Distance.Contains(NeighborTile) && CurrentDistance + 1 <= Range)
            {
                FTileNode NextNode;
                NextNode.Tile = NeighborTile;
                NextNode.Distance = CurrentDistance + 1;

                TileQueue.Enqueue(NextNode);
                Distance.Add(NeighborTile, CurrentDistance + 1);
                AttackableTiles.Add(NeighborTile);
            }
        }
    }
    return AttackableTiles;
}

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
            // 4. Check Validity, Distance, and Empty Status
            if (NeighborTile->GetTileStatus() == ETileStatus::EMPTY && !Distance.Contains(NeighborTile) && CurrentDistance + 1 <= Range)
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
    return ReachableTiles;
}

ATile* AAW_Brawler::GetTileIsOnNow() const
{
    return TileIsOnNow;
}

void AAW_Brawler::SetTileIsOnNow(ATile* NewTile)
{
    TileIsOnNow = NewTile;
    this->AttachToActor(NewTile, FAttachmentTransformRules::KeepWorldTransform);
}


int32 AAW_Brawler::GetMaxHealth() const
{
    return StartingHealth;
}


void AAW_Brawler::MoveUnit(ATile* TargetTile)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !TargetTile) return;

    // Get grid coordinates
    FString FromCell = TileIsOnNow ? TileIsOnNow->GetGridCoordinatesAsString() : "Unknown";
    FString ToCell = TargetTile->GetGridCoordinatesAsString();

    // Log the move
    FString PlayerID = (OwnerPlayerId == 0) ? "HP" : "AI";
    GameMode->LogMove(PlayerID, "B", FromCell, ToCell);

    // Existing movement logic
    MovementPath = GameMode->GameField->FindPath(TileIsOnNow, TargetTile);
    if (MovementPath.Num() <= 0) return;

    MovingCurrentTile = TileIsOnNow;
    MovingTargetTile = TargetTile;
    bIsMoving = true;
    CurrentPathIndex = 1;

    // Notify UI update
    if (AAWPlayerController* PC = Cast<AAWPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PC->UpdateMoveHistoryUI();
    }
}

void AAW_Brawler::Shoot(ATile* TargetTile)
{
    if (!TargetTile) return;

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode) return;

    // Get target coordinates
    FString TargetCell = TargetTile->GetGridCoordinatesAsString();

    // Calculate damage and log attack
    int32 Damage = FMath::RandRange(MeleeAttackDamage.GetMin(), MeleeAttackDamage.GetMax());
    FString PlayerID = (OwnerPlayerId == 0) ? "HP" : "AI";
    GameMode->LogAttack(PlayerID, "B", TargetCell, Damage);

    // Existing attack logic
    if (IAW_BaseSoldier* TargetSoldier = Cast<IAW_BaseSoldier>(TargetTile->GetUnit()))
    {
        if (TargetSoldier->GetOwnerPlayerId() != OwnerPlayerId)
        {
            TargetSoldier->TakeDamage(Damage);
        }
    }
    // Notify UI update
    if (AAWPlayerController* PC = Cast<AAWPlayerController>(GetWorld()->GetFirstPlayerController()))
    {
        PC->UpdateMoveHistoryUI();
    }
}