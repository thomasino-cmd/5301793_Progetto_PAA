#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AWPlayerController.generated.h"


class UInputMappingContext;
class UInputAction;


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

 
    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
    UInputAction* CoinFlipAction;


    bool bIsMovingUnit;
    bool bHasAttacked;

    void ClickOnGrid();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;




};