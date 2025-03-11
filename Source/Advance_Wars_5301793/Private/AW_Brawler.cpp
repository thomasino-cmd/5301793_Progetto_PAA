// Fill out your copyright notice in the Description page of Project Settings.

#include "AW_Brawler.h"
#include "Tile.h"

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

    // Create the scene component
    Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
    SetRootComponent(Scene); // Set the scene component as the root component

    // Create the static mesh component
    BrawlerStaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
    BrawlerStaticMeshComponent->SetupAttachment(Scene); // Attach the static mesh component to the scene component

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