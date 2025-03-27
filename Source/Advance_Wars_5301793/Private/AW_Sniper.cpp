// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Sniper.h"
#include "Tile.h"
#include "AWGameMode.h"
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
   


    MoveSpeed = 100.0f;            // Example speed
    MovingTargetTile = nullptr; 
    bIsMoving = false;
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

}

// Called when the game starts or when spawned
void AAW_Sniper::BeginPlay()
{
    Super::BeginPlay();
}


int32 AAW_Sniper::GetOwnerPlayerId() const
{
	return OwnerPlayerId;
}


void AAW_Sniper::Tick(float DeltaTime)
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
            //OnMoveCompleted1.Broadcast(); // Trigger the event        
        }
    }
}
















void AAW_Sniper::Attack()
{


}

void AAW_Sniper::TakeDamage(float Damage)
{
    Health -= Damage;
    FString DamageString = FString::Printf(TEXT("%f"), Damage);
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Subiti %s danni :/ "), *DamageString));

    if (Health <= 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("vita esaurita. morte "));

        AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
        if (GameMode)
        {
            // Remove from appropriate array
            if (OwnerPlayerId == 0)
            {
                GameMode->Player1Snipers.Remove(this);
            }
            else if (OwnerPlayerId == 1)
            {
                GameMode->Player2Snipers.Remove(this);
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

TArray<ATile*> AAW_Sniper::GetAttackableTiles()
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

ATile* AAW_Sniper::GetTileIsOnNow() const
{
    return TileIsOnNow;
}

void AAW_Sniper::SetTileIsOnNow(ATile* NewTile)
{
    TileIsOnNow = NewTile;
    this->AttachToActor(NewTile, FAttachmentTransformRules::KeepWorldTransform);
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





void AAW_Sniper::MoveUnit(ATile* TargetTile)
{

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    //this->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
    
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



void AAW_Sniper::Shoot(ATile* TargetTile)
{
    if (!TargetTile) return;

    AActor* OccupyingActor = TargetTile->GetUnit();
    IAW_BaseSoldier* TargetSoldier = Cast<IAW_BaseSoldier>(OccupyingActor);

    if (!TargetSoldier)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Nessun bersaglio valido sulla tile."));
        return;
    }

    // Controlla se il bersaglio è un nemico
    if (TargetSoldier->GetOwnerPlayerId() == this->GetOwnerPlayerId()) // Sostituisci con la tua logica
    {
       return;
    }

    // Genera un danno casuale nel range 4-8
    int Damage = FMath::RandRange(4, 8);
   
    // Applica il danno al bersaglio
    TargetSoldier->TakeDamage(Damage);


    // Gestione del contraccolpo:
    bool bCounterattack = false;

    // Calcola la distanza
    float Distance = 9999.f;
    ATile* AttackerTile = this->GetTileIsOnNow();
    if (AttackerTile)
    {
        FVector2D AttackerPos = AttackerTile->GetGridPosition();
        FVector2D TargetPos = TargetTile->GetGridPosition();
        Distance = FMath::Abs(AttackerPos.X - TargetPos.X) + FMath::Abs(TargetPos.Y - TargetPos.Y);
    }

    // Se il bersaglio è uno Sniper, il contraccolpo è sempre applicato
    if (Cast<AAW_Sniper>(TargetSoldier))
    {
        bCounterattack = true;
    }
    // Se il bersaglio è un Brawler, il contraccolpo si applica solo se è a distanza 1
    else if (AAW_Brawler* TargetBrawler = Cast<AAW_Brawler>(TargetSoldier))
    {
        if (Distance <= 1.f)
        {
            bCounterattack = true;
        }
    }

    // Se si applica il contraccolpo, genera un danno random nel range 1-3 e applicalo allo Sniper stesso
    if (bCounterattack)
    {
        int CounterDamage = FMath::RandRange(1, 3);
        UE_LOG(LogTemp, Log, TEXT("%s subisce contraccolpo con danno %d"), *GetName(), CounterDamage);
        this->TakeDamage(CounterDamage);
    }
}