#include "AWGameInstance.h"
#include "HumanPlayer.h"
#include "GameField.h"
#include "Kismet/GameplayStatics.h"


void UAWGameInstance::EndGame()
{

}



FString UAWGameInstance::GetTurnMessage()
{
    return CurrentTurnMessage;
}

void UAWGameInstance::SetTurnMessage(FString Message)
{
    CurrentTurnMessage = Message; 
}
