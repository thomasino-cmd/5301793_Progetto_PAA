// Fill out your copyright notice in the Description page of Project Settings.

#include "AWPlayerController.h"
#include "Components/InputComponent.h"
#include "Kismet/GameplayStatics.h"  // Comment out for now
#include "Engine/World.h" // Include the world header


AAWPlayerController::AAWPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bIsMovingUnit = false;
    bHasAttacked = false;
}

void AAWPlayerController::BeginPlay()
{
    Super::BeginPlay();

    // Get the GameInstance
    //GameInstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld())); // Comment out for now

    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
    {
        Subsystem->AddMappingContext(InputContext, 0);
    }
}

void AAWPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // Get the InputComponent
    UInputComponent* InputComponent = GetInputComponent(); // Get the actual InputComponent

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AAWPlayerController::OnClick);
    }
}

void AAWPlayerController::OnClick()
{
    /*
    if (!GameInstance->IsPlayerTurn()) // Comment out for now
    {
        return;
    }
    */

    FVector2D MousePosition;
    GetMousePosition(MousePosition.X, MousePosition.Y);

    FIntPoint GridCoordinates = GetGridCoordinatesFromScreenLocation(MousePosition);

    // Convert grid coordinates to world location
    //FVector WorldLocation = GameInstance->GetGameField()->GetTileWorldLocation(GridCoordinates.X, GridCoordinates.Y); // Comment out for now

    // Line trace to find the clicked tile
    FHitResult HitResult;
    // Implement the line trace here, similar to how it's done in other parts of your code
    // ...
}

// ... (rest of the method implementations)