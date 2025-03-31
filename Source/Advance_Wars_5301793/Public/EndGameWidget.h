#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "EndGameWidget.generated.h"

class UTextBlock;
class UButton;

UCLASS()
class ADVANCE_WARS_5301793_API UEndGameWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Funzione per impostare il testo del vincitore
    UFUNCTION(BlueprintCallable, Category = "Game Over")
    void SetWinnerText(int32 WinningPlayerId);

protected:
    virtual void NativeConstruct() override;

    // Bind dei widget
    UPROPERTY(meta = (BindWidget))
    UTextBlock* WinnerText;

    //UPROPERTY(meta = (BindWidget))
    //UButton* PlayAgainButton;

private:
    UFUNCTION()
    void OnPlayAgainClicked();
};