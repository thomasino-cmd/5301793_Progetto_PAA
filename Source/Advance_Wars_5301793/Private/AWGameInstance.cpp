#include "AWGameInstance.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "Kismet/GameplayStatics.h"


void UAWGameInstance::EndGame()
{
    // ... Handle game end logic ...
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