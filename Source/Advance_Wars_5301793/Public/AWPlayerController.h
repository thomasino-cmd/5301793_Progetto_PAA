// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
//#include "AWGameInstance.h"  // Comment out GameInstance header for now
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AWPlayerController.generated.h" 

UCLASS()
class ADVANCE_WARS_5301793_API AAWPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    AAWPlayerController();

    UPROPERTY(EditAnywhere, Category = Input)
    UInputMappingContext* InputContext;

    UPROPERTY(EditAnywhere, Category = Input)
    UInputAction* ClickAction;

    UFUNCTION()
    void OnClick();

    // Remove RightClickAction

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    UPROPERTY()
    class AHumanPlayer* SelectedUnit;

    UPROPERTY()
    class ATile* DestinationTile;

    UPROPERTY()
    class ATile* StartingTile;

    //class UAWGameInstance* GameInstance;  // Comment out GameInstance variable

    bool bIsMovingUnit;

    bool bHasAttacked;

    FIntPoint GetGridCoordinatesFromScreenLocation(const FVector2D& ScreenLocation);

    void SelectUnit(class AHumanPlayer* Unit);

    void MoveUnit(class ATile* Tile);

    void AttackUnit(class AHumanPlayer* Unit);

    void DeselectUnit();

    void ShowPossibleMovements();

    void ClearPossibleMovements();

    void ShowPossibleAttacks();

    void ClearPossibleAttacks();
};