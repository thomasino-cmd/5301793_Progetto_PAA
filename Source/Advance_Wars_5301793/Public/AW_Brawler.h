// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AW_BaseSoldier.h" // Include the interface header
#include "AW_Delegates.h"
#include "Delegates/Delegate.h"

#include "AW_Brawler.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);

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


    //FOnMoveCompleted OnMoveCompleted0;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 OwnerPlayerId;


    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Position")
    ATile* TileIsOnNow;


    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    TArray<class ATile*> TilesCanReach;
    
    // Scene component for the tile
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    USceneComponent* Scene;

    // Static mesh component for the tile's visual representation
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UStaticMeshComponent* BrawlerStaticMeshComponent;


    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

  

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;


    
    virtual void Attack() override;

    virtual void TakeDamage(float Damage) override;
    virtual float GetHealth() const override;

    virtual int32 GetMovementRange() const override;


    virtual int32 GetAttackRange() const override;

    virtual TArray<ATile*> GetAttackableTiles() override;

    // Function to get the reachable cells based on MovementRange
    virtual TArray<class ATile*> GetReachableTiles(int32 Range) override;

    virtual ATile* GetTileIsOnNow() const override;

    virtual void SetTileIsOnNow(ATile* NewTile) override;


    virtual int32 GetOwnerPlayerId() const override;


    UFUNCTION(BlueprintCallable)
    void MoveUnit(ATile* TargetTile) ;


    int32 GetMaxHealth() const;
    int32 StartingHealth;

    UFUNCTION(BlueprintCallable)
    void Shoot(ATile* TargetTile);


    
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    TArray<ATile*> MovementPath;


    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    bool bIsMoving;

    ATile* MovingCurrentTile;
    ATile* MovingTargetTile;
    float MoveSpeed;
    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    TArray<ATile*> CurrentPath;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    int32 CurrentPathIndex;


    

};