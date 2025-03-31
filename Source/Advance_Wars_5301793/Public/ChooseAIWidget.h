// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChooseAIWidget.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCE_WARS_5301793_API UChooseAIWidget : public UUserWidget
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable)
    void SelectRandomAI();

    UFUNCTION(BlueprintCallable)
    void SelectAStarAI();

};
