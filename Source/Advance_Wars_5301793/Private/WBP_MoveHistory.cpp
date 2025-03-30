// Fill out your copyright notice in the Description page of Project Settings.

#include "WBP_MoveHistory.h"
#include "Components/TextBlock.h"  
#include "Algo/Reverse.h" 
#include "Components/ListView.h"
#include "HistoryEntry.h"
#include "MoveHistoryEntryObject.h"






void UMoveHistoryWidget::ReverseItemOrder(TArray<UObject*>& Items) const
{
    Algo::Reverse(Items); // Inverte l'ordine dell'array
}






void UMoveHistoryWidget::UpdateHistory(const TArray<UObject*>& NewEntries)
{
    if (!HistoryList || !IsValid(this)) return;

    // Esegui nel game thread se necessario
    if (!IsInGameThread())
    {
        AsyncTask(ENamedThreads::GameThread, [this, NewEntries]()
            {
                UpdateHistory(NewEntries);
            });
        return;
    }

    // 1. Filtra e rimuovi duplicati
    TArray<UObject*> FilteredEntries;
    TSet<UObject*> UniqueEntries;

    for (UObject* Entry : NewEntries)
    {
        if (IsValid(Entry) && !UniqueEntries.Contains(Entry))
        {
            UniqueEntries.Add(Entry);
            FilteredEntries.Add(Entry);
        }
    }

    // 2. Ottieni elementi correnti (senza duplicati)
    TArray<UObject*> CurrentItems = HistoryList->GetListItems();
    CurrentItems.RemoveAll([&](UObject* Item) { return !IsValid(Item); });

    // 3. Aggiungi nuovi elementi in testa
    for (UObject* Entry : FilteredEntries)
    {
        CurrentItems.Insert(Entry, 0);
    }

    // 4. Rimuovi duplicati dalla lista finale
    TArray<UObject*> FinalItems;
    TSet<UObject*> FinalUniqueItems;

    for (UObject* Item : CurrentItems)
    {
        if (IsValid(Item) && !FinalUniqueItems.Contains(Item))
        {
            FinalUniqueItems.Add(Item);
            FinalItems.Add(Item);
        }
    }

    // 5. Limita a MAX_HISTORY_ENTRIES elementi mantenendo i più recenti
    while (FinalItems.Num() > MAX_HISTORY_ENTRIES)
    {
        FinalItems.RemoveAt(FinalItems.Num() - 1); // Rimuove l'elemento più vecchio (in fondo alla lista)
    }

    HistoryList->SetListItems(FinalItems);
    HistoryList->RequestRefresh();
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