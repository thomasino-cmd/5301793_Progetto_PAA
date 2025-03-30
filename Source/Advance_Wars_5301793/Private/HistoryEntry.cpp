// Fill out your copyright notice in the Description page of Project Settings.


#include "HistoryEntry.h"
#include "MoveHistoryManager.h"
#include "WBP_MoveHistory.h"

void UHistoryEntry::SetupEntry(const FString& Text)
{
	if (EntryText) EntryText->SetText(FText::FromString(Text));
}


void UHistoryEntry::NativeOnListItemObjectSet(UObject* ListItemObject)
{

    if (!IsValid(ListItemObject))
    {
        UE_LOG(LogTemp, Error, TEXT("Invalid list item!"));
        return;
    }

    if (UMoveEntry* Entry = Cast<UMoveEntry>(ListItemObject))
    {
        FString DisplayText;
        if (Entry->MoveType == EMoveType::Move)
        {
            DisplayText = FString::Printf(TEXT("%s %s moved from %s to %s"),
                *Entry->PlayerID,
                *Entry->UnitID,
                *Entry->FromCell,
                *Entry->ToCell);
        }
        else
        {
            DisplayText = FString::Printf(TEXT("%s %s attacked %s (Damage: %d)"),
                *Entry->PlayerID,
                *Entry->UnitID,
                *Entry->TargetCell,
                Entry->Damage);
        }
        EntryText->SetText(FText::FromString(DisplayText));

        // Imposta il colore del testo in base al PlayerID
        if (EntryText)
        {
            FLinearColor TextColor;
            if (Entry->PlayerID == "HP") // Player umano
            {
                TextColor = FLinearColor(0.0f, 0.0f, 1.0f, 1.0f); // Blu
            }
            else // IA
            {
                TextColor = FLinearColor(1.0f, 0.0f, 0.0f, 1.0f); // Rosso
            }

            EntryText->SetColorAndOpacity(TextColor);
        }
    }
}