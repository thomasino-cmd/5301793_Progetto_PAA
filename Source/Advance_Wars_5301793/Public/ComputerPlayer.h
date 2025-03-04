// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInterface.h"
#include "AWGameInstance.h"
#include "AWGameMode.h"
#include "GameField.h"
#include "ComputerPlayer.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API AComputerPlayer : public APawn, public IPlayerInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    AComputerPlayer();

protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Implement interface functions (OnTurn, OnWin, OnLose)
    virtual void OnTurn() override;
    virtual void OnWin() override;
    virtual void OnLose() override;

    // Funzioni specifiche per l'IA
    void MakeMove();

    // Funzione per ottenere un riferimento a GameInstance
    UAWGameInstance* GetGameInstance();

    // Funzione per ottenere un riferimento a GameMode
    AAWGameMode* GetGameMode();

private:
    // Funzioni di supporto per l'IA (es. valutazione della situazione, scelta del bersaglio)
    // ...

    // Puntatore a GameField
    AGameField* GameField;
};