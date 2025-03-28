#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "UObject/Class.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "AW_Brawler.h"
#include "AW_Sniper.h"
#include "Coin.h"
#include "WBP_MoveHistory.h"
#include "MoveHistoryManager.h"
#include "AWGameMode.generated.h"


class AActor;

struct FPosition;

UCLASS()
class ADVANCE_WARS_5301793_API AAWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    // Arrays to store player units
    TArray<AAW_Brawler*> Player1Brawlers;
    TArray<AAW_Sniper*> Player1Snipers;
    TArray<AAW_Brawler*> Player2Brawlers;
    TArray<AAW_Sniper*> Player2Snipers;

    int32 Player1UnitsPlaced = 0;
    int32 Player2UnitsPlaced = 0;



    UPROPERTY()
    class UInGameHUDWidget* InGameHUD;


    int32 WinningPlayerId = 0 ;

    // tracks if the game is over
    bool bIsGameOver;

    bool bIsPlacementPhaseOver;
  
    bool bInCoinTossPhase  = false;

    bool bCoinFlipCompleted = false;

    // array of player interfaces
    TArray<IPlayerInterface*> Players;


    // tracks the number of moves in order to signal a drawn game
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 MoveCounter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 UnitsPlaced; 

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 TotalUnitsToPlace;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 CurrentPlayer;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 TurnNumber;



    // Funzione per registrare una mossa (chiamata da PlayerController/AI)
    UFUNCTION(BlueprintCallable, Category = "Move History")
    void LogMove(const FString& PlayerID, const FString& UnitID, const FString& FromCell, const FString& ToCell);

    // Funzione per registrare un attacco
    UFUNCTION(BlueprintCallable, Category = "Move History")
    void LogAttack(const FString& PlayerID, const FString& UnitID, const FString& TargetCell, int32 Damage);


    AAWGameMode();

    virtual void BeginPlay() override;

    //UFUNCTION(BlueprintCallable)
    //void StartCoinTossPhase();


    bool IsInCoinTossPhase() const;

    //UFUNCTION()
    //void InitializeGameSetup();



    UFUNCTION()
    ACoin* GetCoinActor() const;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AGameField> GameFieldClass;

    UPROPERTY(VisibleAnywhere)
    AGameField* GameField;
    
    UFUNCTION(BlueprintCallable, Category = "Game Logic")
    void GetNextPlayer();


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
    int32 FieldSize;

    UFUNCTION(BlueprintCallable)
    void EndTurn();

    UFUNCTION(BlueprintCallable)
    void MoveUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    UFUNCTION(BlueprintCallable)
    void AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);


    void SetUnitPlacement(const int32 PlayerNumber, const FVector& GridPosition);


    UFUNCTION(BlueprintCallable)
    TArray<AActor*> GetCurrentPlayerUnits(int32 PlayerId);



    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Brawler> BrawlerClassHuman;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Sniper> SniperClassHuman;


    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Brawler> BrawlerClassAI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Sniper> SniperClassAI;


    UFUNCTION()
    void SwitchPlayer();

    UFUNCTION()
    bool CheckWinCondition();

    UFUNCTION()
    void EndGame();

    ////////////////////// WIDGET ///////////////////////

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UInGameHUDWidget> InGameHUDClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Player1Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Player2Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 TotalMatchesPlayed = 0;

    // Salva/carica i punteggi
    UFUNCTION(BlueprintCallable)
    void SaveScores();

    UFUNCTION(BlueprintCallable)
    void LoadScores();

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMoveHistoryWidget> MoveHistoryWidgetClass;

    UMoveHistoryWidget* MoveHistoryWidget;

    // Sostituisci la dichiarazione esistente con:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Move History")
    TObjectPtr<UMoveHistoryManager> MoveHistoryManager;

    /// ///////////////COIN FLIP////////////////////////


    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void StartGameSequence();

    UFUNCTION()
    void OnCoinFlipComplete(int32 StartingPlayerIndex);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACoin> CoinClass;

    UFUNCTION()
    void HandleCoinFlipInput();

    void UpdateHUD();


private:
    void SpawnCoinForFlip();

    //void SetupCoinFlipInput();
    
    void InitializeGameplay();
    void StartFirstTurn();


    UPROPERTY()
    ACoin* CoinActor;



    
};