// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
//#include "MoveEntry.h"
//#include "../../../Intermediate/ProjectFiles/MoveEntry.h"
#include "MoveHistoryManager.h"
#include "MoveHistoryEntryObject.h"
#include "HistoryEntry.h"
#include "WBP_MoveHistory.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCE_WARS_5301793_API UMoveHistoryWidget : public UUserWidget
{
	GENERATED_BODY()
	

public:
    UPROPERTY(meta = (BindWidget))
    class UListView* HistoryList;

    UFUNCTION(BlueprintCallable)
    void UpdateHistory(const TArray<UObject*>& Entries);

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
    TSubclassOf<UHistoryEntry> EntryWidgetClass;

    UFUNCTION()
    FString CreateDisplayString(UMoveEntry* Entry);

private:
    static const int32 MAX_HISTORY_ENTRIES = 5;
    void ReverseItemOrder(TArray<UObject*>& Items) const;

    
};
