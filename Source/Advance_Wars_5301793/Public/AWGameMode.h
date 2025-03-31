#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "UObject/Class.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "AW_Brawler.h"
#include "AW_Sniper.h"
#include "Coin.h"
#include "InGameHUDWidget.h"
#include "EndGameWidget.h"
#include "WBP_MoveHistory.h"
#include "MoveHistoryManager.h"
#include "ChooseAIWidget.h"
#include "AWGameMode.generated.h"


class AActor;

struct FPosition;

UCLASS()
class ADVANCE_WARS_5301793_API AAWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAWGameMode();

    virtual void BeginPlay() override;

    // array of player interfaces
    TArray<IPlayerInterface*> Players;

    // Arrays to store player units
    TArray<AAW_Brawler*> Player1Brawlers;
    TArray<AAW_Sniper*> Player1Snipers;
    TArray<AAW_Brawler*> Player2Brawlers;
    TArray<AAW_Sniper*> Player2Snipers;

    int32 Player1UnitsPlaced = 0;
    int32 Player2UnitsPlaced = 0;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Brawler> BrawlerClassHuman;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Sniper> SniperClassHuman;


    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Brawler> BrawlerClassAI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Sniper> SniperClassAI;


    ///////////////////////// AI /////////////////////////

    UFUNCTION(BlueprintCallable)
    void SetAIType(bool bUseAStar);

    UFUNCTION(BlueprintCallable)
    void StartGameAfterSelection();


    UPROPERTY(BlueprintReadOnly)
    bool bUseAStarAI = false;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UChooseAIWidget> ChooseAIWidgetClass;

    UPROPERTY()
    UChooseAIWidget* ChooseAIWidget;


    // tracks if the game is over
    int32 WinningPlayerId = 0 ;

    bool bIsGameOver;

    bool bIsPlacementPhaseOver;
  
    bool bInCoinTossPhase  = false;

    bool bCoinFlipCompleted = false;

    bool IsInCoinTossPhase() const;

    void InitializeGameplay();

    void StartFirstTurn();



    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AGameField> GameFieldClass;

    UPROPERTY(VisibleAnywhere)
    AGameField* GameField;
    

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
    int32 FieldSize;


    /// ///////////////////////// UNIT PLACEMENT /////////////////////////


    void SetUnitPlacement(const int32 PlayerNumber, const FVector& GridPosition);


    UFUNCTION(BlueprintCallable)
    TArray<AActor*> GetCurrentPlayerUnits(int32 PlayerId);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 UnitsPlaced; 

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 TotalUnitsToPlace;

    ////////////////////////////// GAME FLOW //////////////////////////////

    UFUNCTION()
    void SwitchPlayer();

    UFUNCTION(BlueprintCallable, Category = "Game Logic")
    void GetNextPlayer();

    UFUNCTION()
    bool CheckWinCondition();

    UFUNCTION(BlueprintCallable)
    void EndTurn();

    UFUNCTION()
    void EndGame();

    UFUNCTION(BlueprintCallable)
    void RestartGame();


    // tracks the number of moves in order to signal a drawn game
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 MoveCounter;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 CurrentPlayer;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
    int32 TurnNumber;

    ////////////////////// WIDGET ///////////////////////


    UPROPERTY()
    class UInGameHUDWidget* InGameHUD;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<class UInGameHUDWidget> InGameHUDClass;

    void UpdateHUD();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Player1Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 Player2Score = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
    int32 TotalMatchesPlayed = 0;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UEndGameWidget> GameOverWidgetClass;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    TObjectPtr<UEndGameWidget> GameOverWidget;
   

    //////////////////MOVE HISTORY////////////////////////


    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Move History")
    TObjectPtr<UMoveHistoryManager> MoveHistoryManager;

    UPROPERTY(EditDefaultsOnly, Category = "UI")
    TSubclassOf<UMoveHistoryWidget> MoveHistoryWidgetClass;

    UPROPERTY(VisibleAnywhere, Category = "UI")
    TObjectPtr<UMoveHistoryWidget> MoveHistoryWidget; 

    // Funzione per registrare una mossa (chiamata da PlayerController/AI)
    UFUNCTION(BlueprintCallable, Category = "Move History")
    void LogMove(const FString& PlayerID, const FString& UnitID, const FString& FromCell, const FString& ToCell);

    // Funzione per registrare un attacco
    UFUNCTION(BlueprintCallable, Category = "Move History")
    void LogAttack(const FString& PlayerID, const FString& UnitID, const FString& TargetCell, int32 Damage);


    /// ///////////////COIN FLIP////////////////////////

    UFUNCTION()
    ACoin* GetCoinActor() const;

    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void StartGameSequence();

    UFUNCTION()
    void OnCoinFlipComplete(int32 StartingPlayerIndex);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACoin> CoinClass;

    UFUNCTION()
    void HandleCoinFlipInput();

    void SpawnCoinForFlip();

    UPROPERTY()
    ACoin* CoinActor;



    
};