// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include <Components/HorizontalBox.h>
#include "InGameHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCE_WARS_5301793_API UInGameHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable)
    void UpdateTurnIndicator(bool bIsHumanTurn);

    UFUNCTION(BlueprintCallable)
    void UpdateHealthBars(const TArray<float>& CurrentHealthValues, const TArray<float>& MaxHealthValues);


    //UFUNCTION(BlueprintCallable)
    //void UpdateUnitHealth(int32 UnitIndex, float CurrentHealth, float MaxHealth);

    //UFUNCTION(BlueprintCallable)
    //void UpdateScore(int32 PlayerScore, int32 AIScore);

    // Esponi le properties per il binding
    UPROPERTY(meta = (BindWidget))
    UTextBlock* Text_TurnStatus;

    // Bind delle ProgressBar
    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar_Brawler1;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar_Brawler2;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar_Sniper1;

    UPROPERTY(meta = (BindWidget))
    class UProgressBar* HealthBar_Sniper2;


    // ... aggiungi tutti gli altri elementi qui ...
};
