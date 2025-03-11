// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AW_BaseSoldier.h" // Include the interface header
#include "AW_Brawler.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API AAW_Brawler : public AActor, public IAW_BaseSoldier
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AAW_Brawler();

    // Range of damage for the melee attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Brawler")
    FIntPoint MeleeAttackDamage;

    // Health of the soldier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    float Health;

    // Movement range of the soldier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    int32 MovementRange;

    // Attack range of the soldier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    int32 AttackRange;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    float MovementSpeed;



    // Scene component for the tile
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    USceneComponent* Scene;

    // Static mesh component for the tile's visual representation
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* BrawlerStaticMeshComponent;

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Implement interface functions
    virtual void Move(FVector Direction) override;
    virtual void Attack(AActor* Target) override;
    virtual void TakeDamage(float Damage) override;
    virtual float GetHealth() const override;
    virtual int32 GetMovementRange() const override;
    virtual int32 GetAttackRange() const override;

    // Function to get the reachable cells based on MovementRange
    TArray<class ATile*> GetReachableTiles(int32 Range, bool bIgnoreObstacles = false);
};