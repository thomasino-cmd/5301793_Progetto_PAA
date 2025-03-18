// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameField.h"
#include "AW_BaseSoldier.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UAW_BaseSoldier : public UInterface
{
    GENERATED_BODY()
};

/**
 * */
class ADVANCE_WARS_5301793_API IAW_BaseSoldier
{
    GENERATED_BODY()

    // Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

    // Function for movement
    virtual void Move(FVector Direction) {};

    // Function for attack
    virtual void Attack(AActor* Target) {};

    // Function for taking damage
    virtual void TakeDamage(float Damage) {};

    // Function to get current health
    virtual float GetHealth() const = 0;

    // Function to get MovementRange
    virtual int32 GetMovementRange() const = 0;

    // Function to get AttackRange
    virtual int32 GetAttackRange() const = 0;

    // Function to get legal moves
    virtual TArray<FVector2D> GetLegalMoves() const = 0;

    virtual TArray<class ATile*> GetReachableTiles(int32 Range) = 0;

    
    int32 OwnerPlayerId;

    
    ATile* CurrentTile;
};