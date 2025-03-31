#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GameField.h"
#include "AW_BaseSoldier.generated.h"

struct FTileNode
{
    ATile* Tile;
    int32 Distance;
};

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

public:

    virtual void Attack() {};

    virtual void TakeDamage(float Damage) {};

    virtual float GetHealth() const = 0;

    virtual int32 GetMovementRange() const = 0;

    virtual int32 GetAttackRange() const = 0;

    virtual TArray<class ATile*> GetAttackableTiles() =0 ;

    virtual TArray<class ATile*> GetReachableTiles(int32 Range) = 0;

    virtual int32 GetOwnerPlayerId() const = 0;
    
    int32 OwnerPlayerId;
    
    ATile* TileIsOnNow;

    virtual ATile* GetTileIsOnNow() const = 0; 
   
    virtual void SetTileIsOnNow(ATile* NewTile) = 0; 
};