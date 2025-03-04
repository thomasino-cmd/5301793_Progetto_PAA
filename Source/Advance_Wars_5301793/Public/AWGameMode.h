#pragma once

#include "CoreMinimal.h"

#include "GameFramework/GameModeBase.h"
#include "UObject/Class.h"
#include "AWGameMode.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API AAWGameMode : public AGameModeBase
{
    GENERATED_BODY()

public:
    AAWGameMode();

    virtual void BeginPlay() override;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    TSubclassOf<AHumanPlayer> HumanPlayerClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    TSubclassOf<AGameField> GameFieldClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Settings")
    int32 FieldSize;

    UFUNCTION(BlueprintCallable)
    void EndTurn();

    UFUNCTION(BlueprintCallable)
    void MoveUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

    UFUNCTION(BlueprintCallable)
    void AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY);

protected:
    UPROPERTY()
    AHumanPlayer* HumanPlayer;

    UPROPERTY()
    AGameField* GameField;

    UPROPERTY()
    int32 CurrentPlayer;

    UPROPERTY()
    int32 TurnNumber;

    UFUNCTION()
    void InitializeGame();

    UFUNCTION()
    void SwitchPlayer();

    UFUNCTION()
    bool CheckWinCondition();

    UFUNCTION()
    void EndGame();
};