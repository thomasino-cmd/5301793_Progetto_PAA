// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInterface.h"
#include "AWGameInstance.h"
//#include "AWGameMode.h"
#include "GameField.h"
#include "ComputerPlayer.generated.h"

class AAWGameMode; // Forward declaration

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMovementFinished);

UCLASS()
class ADVANCE_WARS_5301793_API AComputerPlayer : public APawn, public IPlayerInterface
{
    GENERATED_BODY()

public:
    // Sets default values for this pawn's properties
    AComputerPlayer();

    //todo add gameistance reference
    UAWGameInstance* GameIstance;

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

    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override; 

    // Funzioni specifiche per l'IA
    UFUNCTION()
    void MakeMove();

    UFUNCTION()
    void MoveAIUnits(TArray<AActor*> AIUnits, int32 UnitIndex, float Delay);

    UFUNCTION()
    void PlaceUnit();

    // Funzioni di supporto per l'IA (es. valutazione della situazione, scelta del bersaglio)
    // ...

    bool bBrawlerMoved = false;
    bool bSniperMoved = false;

private: 
    FTimerHandle MovementTimerHandle;
 
};