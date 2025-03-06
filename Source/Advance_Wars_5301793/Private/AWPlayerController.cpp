#include "AWPlayerController.h"
#include "Components/InputComponent.h"
#include "HumanPlayer.h"
#include "ComputerPlayer.h"
#include "Obstacle.h"
#include "Tile.h"


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

void AAWPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(InputComponent))
    {
        EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AAWPlayerController::OnClick);
    }
}

void AAWPlayerController::OnClick()
{
    // 1. Get mouse position and convert to world coordinates.
    FVector2D MousePosition;
    GetMousePosition(MousePosition.X, MousePosition.Y);

    // 2. Perform line trace to detect clicked tile.
    FHitResult HitResult;
    GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

    if (HitResult.bBlockingHit)
    {
        ATile* ClickedTile = Cast<ATile>(HitResult.GetActor());
        if (ClickedTile)
        {
            AActor* ClickedActor = ClickedTile->GetUnit();

            if (ClickedActor)
            {
                if (AHumanPlayer* ClickedUnit = Cast<AHumanPlayer>(ClickedActor))
                {
                    // Handle human player unit
                    if (!SelectedUnit)
                    {
                        SelectUnit(ClickedUnit);
                        StartingTile = ClickedTile;
                    }
                    // 4. Handle unit movement.
                    else if (bIsMovingUnit && ClickedTile != StartingTile)
                    {
                        MoveUnit(ClickedTile);
                    }
                    // 5. Handle unit attack.
                    else if (SelectedUnit && ClickedTile != StartingTile)
                    {
                        AttackUnit(ClickedUnit);
                    }
                }
                else if (AComputerPlayer* ClickedComputerUnit = Cast<AComputerPlayer>(ClickedActor))
                {
                    // Handle AI player unit (e.g., show info)
                }
                else if (AObstacle* ClickedObstacle = Cast<AObstacle>(ClickedActor))
                {
                    // Handle obstacle (e.g., show info)
                }
                // ... handle other types as needed
            }
            else
            {
                // Handle empty tile (e.g., move selected unit)
                if (SelectedUnit && bIsMovingUnit)
                {
                    MoveUnit(ClickedTile);
                }
            }
        }
    }
}

void AAWPlayerController::ClickOnGrid()
{
    const auto HumanPlayer = Cast<AHumanPlayer>(GetPawn());
    if (IsValid(HumanPlayer))
    {
        HumanPlayer->OnClick();
    }
}

void AAWPlayerController::SelectUnit(AHumanPlayer* Unit)
{
    SelectedUnit = Unit;
    // Add logic to highlight the unit or show movement range
}

void AAWPlayerController::MoveUnit(ATile* Tile)
{
    // Add logic to move the SelectedUnit to the Tile
    // This might involve pathfinding or animation
    bIsMovingUnit = false;
}

void AAWPlayerController::AttackUnit(AHumanPlayer* Unit)
{
    // Add logic for the SelectedUnit to attack the Unit
    // This might involve damage calculation or combat animation
    DeselectUnit();
}

void AAWPlayerController::DeselectUnit()
{
    SelectedUnit = nullptr;
    // Add logic to clear highlighting or hide movement range
}