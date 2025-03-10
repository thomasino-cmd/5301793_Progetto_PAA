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

    UFUNCTION(BlueprintCallable)
    void EndGame();


    UFUNCTION(BlueprintCallable)
    FString GetTurnMessage();

    UFUNCTION(BlueprintCallable)
    void SetTurnMessage(FString Message);

    UPROPERTY(EditAnywhere)
    FString CurrentTurnMessage = "Current Player";

    // ... other functions to manage game state ...
};