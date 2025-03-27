#include "AWPlayerController.h"
#include "Components/InputComponent.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"
#include "Obstacle.h"
#include "Tile.h"
#include "AW_Sniper.h"
#include "AW_Brawler.h"
#include "AWGameMode.h"


AAWPlayerController::AAWPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    
}

void AAWPlayerController::BeginPlay()
{
    Super::BeginPlay();

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputContext, 0);
    }
}

//void AAWPlayerController::SetupInputComponent()
//{
//    Super::SetupInputComponent();
//
//    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
//    {
//        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AAWPlayerController::ClickOnGrid);  
//    }
//}

void AAWPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        // Bind standard per il click sul grid
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AAWPlayerController::ClickOnGrid);

        
    }
}




// CLICKONGRID cosi funziona pero vale finora solo per il posizionamento 


void AAWPlayerController::ClickOnGrid()
{
    const auto HumanPlayer = Cast<AHumanPlayer>(GetPawn());
    if (IsValid(HumanPlayer))
    {
        // Check if the delegate is bound before executing
        if (HumanPlayer->OnClickAction.IsBound())
        {
            HumanPlayer->OnClickAction.Execute();
        }
        else
        {
            // Log an error message for debugging
            UE_LOG(LogTemp, Error, TEXT("OnClickAction delegate is not bound!"));
        }
    }
}

//
//void AAWPlayerController::ClickOnGrid()
//{
//    const auto HumanPlayer = Cast<AHumanPlayer>(GetPawn());
//    if (IsValid(HumanPlayer))
//    {
//        if (HumanPlayer->bWaitingForMoveInput)
//        {
//            HumanPlayer->HandleTileClick(CurrentTile, ReachableTiles); 
//               
//        }
//        else
//        {
//            HumanPlayer->OnClick(); // Chiama OnClick per la gestione normale dei clic
//        }
//    }
//}








