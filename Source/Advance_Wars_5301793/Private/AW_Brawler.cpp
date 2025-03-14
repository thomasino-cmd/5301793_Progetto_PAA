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

    // Ottieni la posizione corrente dalla tile genitore
    ATile* CurrentTile = Cast<ATile>(GetParentActor());
    if (!CurrentTile)
    {
        return LegalMoves;
    }
    FVector2D CurrentPosition = CurrentTile->GetGridPosition();

    // Ottieni un riferimento al GameField
    AGameField* Field = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!Field)
    {
        return LegalMoves;
    }

    int32 Range = GetMovementRange();
    int32 FieldSize = Field->GetSize();

    for (int32 X = 0; X < FieldSize; ++X)
    {
        for (int32 Y = 0; Y < FieldSize; ++Y)
        {
            FVector2D TargetPosition(X, Y);
            float Distance = FVector2D::Distance(CurrentPosition, TargetPosition);

            if (Distance <= Range)
            {
                ATile* TargetTile = Field->GetTile(X, Y);
                if (TargetTile && TargetTile->GetTileStatus() != ETileStatus::OBSTACLE && TargetTile->GetUnit() == nullptr)
                {
                    LegalMoves.Add(TargetPosition);
                }
            }
        }
    }
    return LegalMoves;
}

int32 AAW_Brawler::GetAttackRange() const
{
    return AttackRange;
}

// Function to get the reachable cells based on MovementRange
TArray<class ATile*> AAW_Brawler::GetReachableTiles(int32 Range, bool bIgnoreObstacles)
{
    TArray<ATile*> ReachableTiles;
    // Ottieni la cella corrente dell'unità
    ATile* CurrentTile = Cast<ATile>(GetParentActor());
    if (CurrentTile)
    {
        // TODO: Implementa la logica per trovare le celle raggiungibili in base al Range
        // TODO: Considera gli ostacoli se bIgnoreObstacles è false
        // TODO: Aggiungi le celle raggiungibili a ReachableTiles
    }
    return ReachableTiles;
}