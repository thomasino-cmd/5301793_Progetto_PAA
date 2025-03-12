#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "UObject/Class.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "AW_Brawler.h"
#include "AW_Sniper.h"
#include "AWGameMode.generated.h"


class AActor;

struct FPosition;

UCLASS()
class ADVANCE_WARS_5301793_API AAWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:

    // tracks if the game is over
    bool bIsGameOver;

    bool bIsPlacementPhaseOver;

    // array of player interfaces
    TArray<IPlayerInterface*> Players;


    // tracks the number of moves in order to signal a drawn game
    int32 MoveCounter;

    int32 UnitsPlaced; 
    int32 TotalUnitsToPlace;



    UPROPERTY()
    int32 CurrentPlayer;

    UPROPERTY()
    int32 TurnNumber;


    AAWGameMode();

    virtual void BeginPlay() override;

    void ChoosePlayerAndStartGame();

  

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AGameField> GameFieldClass;

    UPROPERTY(VisibleAnywhere)
    AGameField* GameField;

    int32 GetNextPlayer(int32 Player);


    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Game Settings")
    int32 FieldSize;

    UFUNCTION(BlueprintCallable)
    void EndTurn();

    UFUNCTION(BlueprintCallable)
    void MoveUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    UFUNCTION(BlueprintCallable)
    void AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);


    void SetUnitPlacement(const int32 PlayerNumber, const FVector& GridPosition);

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Brawler> BrawlerClassHuman;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Sniper> SniperClassHuman;


    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Brawler> BrawlerClassAI;

    UPROPERTY(EditDefaultsOnly)
    TSubclassOf<AAW_Sniper> SniperClassAI;

protected:



   


    UFUNCTION()
    void SwitchPlayer();

    UFUNCTION()
    bool CheckWinCondition();

    UFUNCTION()
    void EndGame();


    
};