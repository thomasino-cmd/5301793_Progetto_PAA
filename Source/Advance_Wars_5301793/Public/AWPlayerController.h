#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
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

   // UFUNCTION()
    //    void OnClick();

    

    bool bIsMovingUnit;
    bool bHasAttacked;

    void ClickOnGrid();

    //void SelectUnit(AHumanPlayer* Unit);
    //void MoveUnit(ATile* Tile);
    //void AttackUnit(AHumanPlayer* Unit);
    //void DeselectUnit();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;


};