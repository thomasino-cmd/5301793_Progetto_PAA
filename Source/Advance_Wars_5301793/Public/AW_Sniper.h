// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AW_BaseSoldier.h" 
#include "AW_Delegates.h"
#include "Delegates/Delegate.h"
#include "AW_Sniper.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);


UCLASS()
class ADVANCE_WARS_5301793_API AAW_Sniper : public AActor, public IAW_BaseSoldier 
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AAW_Sniper();

    // Range of damage for the ranged attack
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Sniper")
    FIntPoint RangedAttackDamage;

    // Health of the soldier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    float Health;

    // Movement range of the soldier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    int32 MovementRange;

    // Attack range of the soldier
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Soldier")
    int32 AttackRange;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Unit")
    int32 OwnerPlayerId;

    // Scene component for the tile
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    USceneComponent* Scene;

    // Static mesh component for the tile's visual representation
    UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
    UStaticMeshComponent* BrawlerStaticMeshComponent;

protected:

    virtual void BeginPlay() override;
public:

    virtual void Tick(float DeltaTime) override;

    virtual void Attack() override;
    virtual void TakeDamage(float Damage) override;
    virtual float GetHealth() const override;
    
    // Function to get the reachable cells based on MovementRange
    virtual TArray<class ATile*> GetReachableTiles(int32 Range) override;

    virtual TArray<ATile*> GetAttackableTiles() override;

    int32 GetMaxHealth() const;
    int32 StartingHealth; 

    virtual int32 GetOwnerPlayerId() const override;


    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Position")
    ATile* TileIsOnNow;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
    TArray<class ATile*> TilesCanReach;

    virtual ATile* GetTileIsOnNow() const override;

    virtual void SetTileIsOnNow(ATile* NewTile) override;

    virtual int32 GetMovementRange() const override;

    virtual int32 GetAttackRange() const override;

    UFUNCTION(BlueprintCallable)
    void MoveUnit(ATile* TargetTile) ;

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