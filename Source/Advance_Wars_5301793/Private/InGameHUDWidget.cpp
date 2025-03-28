// Fill out your copyright notice in the Description page of Project Settings.

#include "InGameHUDWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"

void UInGameHUDWidget::UpdateHealthBars(const TArray<float>& CurrentHealthValues, const TArray<float>& MaxHealthValues)
{
    // Ordine atteso: Brawler1, Sniper1, Brawler2, Sniper2
    TArray<UProgressBar*> HealthBars = {
        HealthBar_Brawler1,
        HealthBar_Sniper1,
        HealthBar_Brawler2,
        HealthBar_Sniper2
    };

    // Sicurezza: verifica che gli array abbiano esattamente 4 elementi
    if (CurrentHealthValues.Num() != 4 || MaxHealthValues.Num() != 4)
    {
        UE_LOG(LogTemp, Error, TEXT("UpdateHealthBars: Richiesti 4 valori! Ricevuti %d current e %d max"),
            CurrentHealthValues.Num(), MaxHealthValues.Num());
        return;
    }

    // Aggiorna ogni health bar
    for (int32 i = 0; i < HealthBars.Num(); ++i)
    {
        if (HealthBars[i])
        {
            // Calcola la percentuale (0-1) con sicurezza
            float Percent = (MaxHealthValues[i] > 0.f) ?
                (CurrentHealthValues[i] / MaxHealthValues[i]) : 0.f;

            HealthBars[i]->SetPercent(Percent);

            // Debug
            UE_LOG(LogTemp, Verbose, TEXT("Barra %d: %.1f/%.1f (%.0f%%)"),
                i, CurrentHealthValues[i], MaxHealthValues[i], Percent * 100.f);
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

void UInGameHUDWidget::UpdateScoreDisplay(int32 Player1, int32 Player2, int32 TotalMatches)
{
    if (Text_Player1Score && Text_Player2Score && Text_TotalMatches)
    {
        Text_Player1Score->SetText(FText::AsNumber(Player1));
        Text_Player2Score->SetText(FText::AsNumber(Player2));
        Text_TotalMatches->SetText(FText::Format(
            FText::FromString("Matches: {0}"),
            FText::AsNumber(TotalMatches)
        ));
    }
}