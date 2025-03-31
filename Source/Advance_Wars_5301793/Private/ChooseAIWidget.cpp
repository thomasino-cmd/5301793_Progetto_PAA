// Fill out your copyright notice in the Description page of Project Settings.


#include "ChooseAIWidget.h"
#include "AWGameMode.h"

void UChooseAIWidget::SelectRandomAI()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->SetAIType(false); // false = Random AI
        GameMode->StartGameAfterSelection();
    }
}

void UChooseAIWidget::SelectAStarAI()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->SetAIType(true); // true = A* AI
        GameMode->StartGameAfterSelection();
    }
}


