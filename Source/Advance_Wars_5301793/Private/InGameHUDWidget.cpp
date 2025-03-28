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

    // Verifica integrità dati
    if (CurrentHealthValues.Num() != 4 || MaxHealthValues.Num() != 4)
    {
        UE_LOG(LogTemp, Error, TEXT("Dati salute non validi! Attesi 4 valori, ricevuti %d/%d"),
            CurrentHealthValues.Num(), MaxHealthValues.Num());
        return;
    }

    // Aggiorna ogni barra
    for (int32 i = 0; i < HealthBars.Num(); ++i)
    {
        if (HealthBars[i])
        {
            // Calcolo sicuro della percentuale
            float Percent = FMath::Clamp(CurrentHealthValues[i] / FMath::Max(1.f, MaxHealthValues[i]), 0.f, 1.f);
            HealthBars[i]->SetPercent(Percent);
        }
    }
}

void UInGameHUDWidget::UpdateTurnIndicator(bool bIsHumanTurn)
{
    if (Text_TurnStatus)
    {
        Text_TurnStatus->SetText(bIsHumanTurn ?
            FText::FromString("HUMAN TURN") :
            FText::FromString("AI TURN"));

        Text_TurnStatus->SetColorAndOpacity(bIsHumanTurn ?
            FSlateColor(FColor::Blue) :
            FSlateColor(FColor::Red));
    }
}

void UInGameHUDWidget::UpdateScoreDisplay(int32 Player1, int32 Player2, int32 TotalMatches)
{
    //if (Text_Player1Score && Text_Player2Score && Text_TotalMatches)
    //{
    //    Text_Player1Score->SetText(FText::AsNumber(Player1));
    //    Text_Player2Score->SetText(FText::AsNumber(Player2));
    //    Text_TotalMatches->SetText(FText::Format(
    //        FText::FromString("Matches: {0}"),
    //        FText::AsNumber(TotalMatches)
    //    ));
    //}
}