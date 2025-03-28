#include "MoveHistoryManager.h"

void UMoveHistoryManager::LogMove(const FString& PlayerID, const FString& UnitID, const FString& From, const FString& To)
{
    UMoveEntry* Entry = NewObject<UMoveEntry>(this);
    Entry->PlayerID = PlayerID;
    Entry->UnitID = UnitID;
    Entry->MoveType = EMoveType::Move;
    Entry->FromCell = From;
    Entry->ToCell = To;
    MoveHistory.Add(Entry);
}

void UMoveHistoryManager::LogAttack(const FString& PlayerID, const FString& UnitID, const FString& Target, int32 Damage)
{
    UMoveEntry* Entry = NewObject<UMoveEntry>(this);
    Entry->PlayerID = PlayerID;
    Entry->UnitID = UnitID;
    Entry->MoveType = EMoveType::Attack;
    Entry->TargetCell = Target;
    Entry->Damage = Damage;
    MoveHistory.Add(Entry);
}