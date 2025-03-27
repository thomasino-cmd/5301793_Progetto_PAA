// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UInGameHUDWidget::UpdateHealthBars(const TArray<float>& CurrentHealthValues, const TArray<float>& MaxHealthValues)
{
    // Ordine atteso: Brawler1, Brawler2, Sniper1, Sniper2
    TArray<UProgressBar*> HealthBars = { HealthBar_Brawler1, HealthBar_Sniper1, HealthBar_Brawler2, HealthBar_Sniper2 };

    for (int32 i = 0; i < HealthBars.Num(); ++i)
    {
        if (HealthBars[i] && MaxHealthValues[i] > 0.f)
        {
            HealthBars[i]->SetPercent(CurrentHealthValues[i] / MaxHealthValues[i]);

           
        }
    }
}

void UInGameHUDWidget::UpdateTurnIndicator(bool bIsHumanTurn)
{
    if (Text_TurnStatus)
    {
        Text_TurnStatus->SetText(bIsHumanTurn ?
            FText::FromString("PLAYER TURN") :
            FText::FromString("AI TURN"));

        Text_TurnStatus->SetColorAndOpacity(bIsHumanTurn ?
            FSlateColor(FColor::Green) :
            FSlateColor(FColor::Red));
    }
}