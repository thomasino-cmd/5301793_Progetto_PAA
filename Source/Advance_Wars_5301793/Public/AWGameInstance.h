// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "AWGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCE_WARS_5301793_API UAWGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
    // Game state variables
    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    bool bIsGameOver;

    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    int32 CurrentPlayerIndex;

    UPROPERTY(BlueprintReadWrite, Category = "Game State")
    int32 TurnNumber;

    // Player information
    UPROPERTY(BlueprintReadWrite, Category = "Players")
    TArray<class AHumanPlayer*> Players;

    // GameField information
    UPROPERTY(BlueprintReadWrite, Category = "Game Field")
    class AGameField* GameField;

    UFUNCTION(BlueprintCallable)
    void EndGame();

    // Functions to manage turns
    UFUNCTION(BlueprintCallable)
    void EndTurn();

    // Functions to interact with units
    UFUNCTION(BlueprintCallable)
    void MoveUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    UFUNCTION(BlueprintCallable)
    void AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    // Functions to get possible moves and attacks for a unit
    UFUNCTION(BlueprintCallable)
    TArray<FVector> GetPossibleMoves(int32 UnitX, int32 UnitY);

    UFUNCTION(BlueprintCallable)
    TArray<FVector> GetPossibleAttacks(int32 UnitX, int32 UnitY);

    UFUNCTION(BlueprintCallable)
    FString GetTurnMessage();

    UFUNCTION(BlueprintCallable)
    void SetTurnMessage(FString Message);

    UPROPERTY(EditAnywhere)
    FString CurrentTurnMessage = "Current Player";

    // ... other functions to manage game state ...
};