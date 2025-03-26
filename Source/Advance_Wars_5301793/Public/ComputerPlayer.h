// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInterface.h"
#include "AWGameInstance.h"
//#include "AWGameMode.h"
#include "GameField.h"
#include "ComputerPlayer.generated.h"



enum class EComputerMoveState
{
    Idle,
    Moving,
    MovingBrawler,
    MovingSniper,
    Finished
};
class AAW_Brawler;
class AAW_Sniper;


class AAWGameMode; // Forward declaration


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
    UFUNCTION()
    void MoveSniper();

    UFUNCTION()
    void ContinueWithNextUnit();


    UFUNCTION()
    void PerformAttack(AActor* AttackingUnit);


    //UFUNCTION()
    //void OnBrawlerMoveCompleted();

    //UFUNCTION()
    //void OnSniperMoveCompleted();

    //void CheckIfBothMoved();


    bool bAttackInProgress;


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
    void MoveBrawler();

    //UFUNCTION()
    //void MoveAIUnits(TArray<AActor*> AIUnits, int32 UnitIndex, float Delay);

    UFUNCTION()
    void PlaceUnit();

    // Funzioni di supporto per l'IA (es. valutazione della situazione, scelta del bersaglio)
    // ...

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool bBrawlerMoved = false;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    bool bSniperMoved = false;


    EComputerMoveState CurrentState = EComputerMoveState::Idle;
    TArray<AActor*> AIUnits;        // Array delle unità correnti dell'AI

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    AAW_Brawler* SelectedBrawler;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    AAW_Sniper* SelectedSniper;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 RandomIndex;
 

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
    int32 RandomTileIndex;

    bool bMoveInProgress;
    bool bTurnEnded;



    bool bBrawlerAttackedThisTurn;
    bool bSniperAttackedThisTurn;

    FTimerHandle BrawlerTimerHandle;
    FTimerHandle SniperTimerHandle;


};