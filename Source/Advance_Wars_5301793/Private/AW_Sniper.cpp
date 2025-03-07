// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Sniper.h"
#include "Tile.h"

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

// Function to get the reachable cells based on MovementRange
TArray<ATile*> AAW_Sniper::GetReachableTiles(int32 Range, bool bIgnoreObstacles)
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