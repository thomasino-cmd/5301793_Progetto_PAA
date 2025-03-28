// Fill out your copyright notice in the Description page of Project Settings.

#include "WBP_MoveHistory.h"
#include "Components/TextBlock.h"  
#include "Components/ListView.h"
#include "HistoryEntry.h"
#include "MoveHistoryEntryObject.h"


//void UMoveHistoryWidget::UpdateHistory(const TArray<FMoveEntry>& Entries)
//{
//    if (!HistoryList || !EntryWidgetClass) return;
//
//    HistoryList->ClearListItems();
//
//    for (const FMoveEntry& Entry : Entries)
//    {
//        // Usa UWBP_HistoryEntry invece di UMoveHistoryEntryObject
//        UHistoryEntry* EntryWidget = CreateWidget<UHistoryEntry>(this, EntryWidgetClass);
//        if (EntryWidget)
//        {
//            EntryWidget->SetupEntry(CreateDisplayString(Entry));
//            HistoryList->AddItem(EntryWidget);
//        }
//    }
//}


void UMoveHistoryWidget::UpdateHistory(const TArray<UObject*>& Entries)
{
    if (HistoryList)
    {
        HistoryList->SetListItems(Entries);
    }
}




FString UMoveHistoryWidget::CreateDisplayString(UMoveEntry* Entry)
{
    if (!Entry) return FString(); // Aggiunto controllo di sicurezza

    if (Entry->MoveType == EMoveType::Move)
    {
        return FString::Printf(TEXT("%s:%s %s -> %s"),
            *Entry->PlayerID,
            *Entry->UnitID,
            *Entry->FromCell,
            *Entry->ToCell);
    }
    else
    {
        return FString::Printf(TEXT("%s:%s attacca %s (%d dmg)"),
            *Entry->PlayerID,
            *Entry->UnitID,
            *Entry->TargetCell,
            Entry->Damage);
    }
}