#include "AWGameInstance.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "Kismet/GameplayStatics.h"


void UAWGameInstance::EndGame()
{
    // ... Handle game end logic ...
}

void UAWGameInstance::EndTurn()
{
    // Switch to the next player
    CurrentPlayerIndex = (CurrentPlayerIndex + 1) % Players.Num();

    // Increment the turn number if the current player is the first player
    if (CurrentPlayerIndex == 0)
    {
        TurnNumber++;
    }

    // ... other actions ...
}

void UAWGameInstance::MoveUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    if (Players.IsValidIndex(CurrentPlayerIndex))
    {
       // Players[CurrentPlayerIndex]->MoveUnit(FromX, FromY, ToX, ToY);
    }
}

void UAWGameInstance::AttackUnit(int32 FromX, int32 FromY, int32 ToX, int32 ToY)
{
    if (Players.IsValidIndex(CurrentPlayerIndex))
    {
       // Players[CurrentPlayerIndex]->AttackUnit(FromX, FromY, ToX, ToY);
    }
}

TArray<FVector> UAWGameInstance::GetPossibleMoves(int32 UnitX, int32 UnitY)
{
    if (Players.IsValidIndex(CurrentPlayerIndex))
    {
       // return Players[CurrentPlayerIndex]->GetPossibleMoves(UnitX, UnitY);
    }
    return TArray<FVector>();
}

TArray<FVector> UAWGameInstance::GetPossibleAttacks(int32 UnitX, int32 UnitY)
{
    if (Players.IsValidIndex(CurrentPlayerIndex))
    {
       // return Players[CurrentPlayerIndex]->GetPossibleAttacks(UnitX, UnitY);
    }
    return TArray<FVector>();
}


FString UAWGameInstance::GetTurnMessage()
{
    return CurrentTurnMessage;
}

void UAWGameInstance::SetTurnMessage(FString Message)
{
    CurrentTurnMessage = Message; 
}

// ... other functions ...