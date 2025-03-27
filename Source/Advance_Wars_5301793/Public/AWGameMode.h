#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "UObject/Class.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "AW_Brawler.h"
#include "AW_Sniper.h"
#include "Coin.h"
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


    // tracks if the game is over
    bool bIsGameOver;

    bool bIsPlacementPhaseOver;
  

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


    AAWGameMode();

    virtual void BeginPlay() override;

    void ChoosePlayerAndStartGame();

  

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


    /// ///////////////COIN FLIP////////////////////////


    UFUNCTION(BlueprintCallable, Category = "Game Flow")
    void StartCoinFlip();

    UFUNCTION()
    void OnCoinFlipResult(int32 StartingPlayerIndex);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<ACoin> CoinClass;

private:
    UPROPERTY()
    ACoin* CoinActor;



   




    
};