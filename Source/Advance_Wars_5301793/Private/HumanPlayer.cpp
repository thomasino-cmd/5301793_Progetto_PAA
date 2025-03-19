#include "HumanPlayer.h"
#include "Tile.h"
#include "Obstacle.h"
#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AW_BaseSoldier.h"
#include "Kismet/GameplayStatics.h"
#include "AWPlayerController.h"


// Sets default values
AHumanPlayer::AHumanPlayer()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Set this pawn to be controlled by the lowest-numbered player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    // Create a camera component
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));

    SetRootComponent(Camera); // Imposta la telecamera come componente radice

    GameIstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    // default init values
    PlayerId = 0;

}

// Called when the game starts or when spawned
void AHumanPlayer::BeginPlay()
{
    Super::BeginPlay();
    
}

// Called every frame
void AHumanPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void AHumanPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AHumanPlayer::OnTurn()
{
    bIsMyTurn = true;
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Your Turn"));
    GameIstance->SetTurnMessage(TEXT("Human Turn"));
    
}

void AHumanPlayer::OnWin()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Win!"));
}

void AHumanPlayer::OnLose()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("You Lose!"));
}





void AHumanPlayer::OnClick()
{


    //Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
    FHitResult Hit = FHitResult(ForceInit);
    // GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());


    if (GameMode->bIsPlacementPhaseOver == false)
    {
        if (Hit.bBlockingHit && bIsMyTurn)
        {
            if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
            {
                if (CurrTile->GetTileStatus() == ETileStatus::EMPTY)
                {
                    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("clicked"));
                    CurrTile->SetTileStatus(PlayerId, ETileStatus::OCCUPIED);
                    FVector SpawnPosition = CurrTile->GetActorLocation();

                    bIsMyTurn = false;

                    // Disabilita l'input PRIMA di chiamare SetUnitPlacement
                    APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();
                    if (PlayerController)
                    {
                        PlayerController->DisableInput(PlayerController);
                    }

                    GameMode->SetUnitPlacement(PlayerId, SpawnPosition);

                    // Riabilita l'input DOPO che SetUnitPlacement (e EndTurn) sono completati
                    if (PlayerController)
                    {
                        // Riabilita input dopo un piccolo delay per evitare problemi, può non servire
                        FTimerHandle InputTimerHandle;
                        float InputDelay = 0.1f;
                        GetWorldTimerManager().SetTimer(InputTimerHandle, [PlayerController]() {
                            PlayerController->EnableInput(PlayerController);
                            }, InputDelay, false);

                    }
                    return;
                }
            }
        }
    }
    else if (GameMode->bIsPlacementPhaseOver == true)
    {
        GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Blue, TEXT("STARTING GAME PHASE"));
        GameIstance->SetTurnMessage(TEXT("STARTING GAME PHASE"));

        if (Hit.bBlockingHit && bIsMyTurn)
        {
            /*if (ATile* ClickedTile = Cast<ATile>(Hit.GetActor()))
            {
                HandleTileClick(ClickedTile);
            }*/
            // Check if a unit was clicked
            if (AActor* ClickedActor = Hit.GetActor())
            {
                IAW_BaseSoldier* ClickedSoldier = Cast<IAW_BaseSoldier>(ClickedActor);
                if (ClickedSoldier)
                {
                    int32 OwnerId = ClickedSoldier->OwnerPlayerId;

                    if (OwnerId == PlayerId)
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Blue, TEXT("TOCCATO UNA TUA UNITA'"));
                        GameIstance->SetTurnMessage(TEXT("TOCCATO UNA TUA UNITA'"));
                        HandleFriendlyUnitClick(ClickedActor); 
                    }
                    else
                    {
                        GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Blue, TEXT("TOCCATO UNA sua UNITA'"));
                        GameIstance->SetTurnMessage(TEXT("TOCCATO UNA sua UNITA'"));
                        HandleEnemyUnitClick(ClickedActor);  
                    }
                }
            }
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("not in a right phase of the game"))
    }
}


void AHumanPlayer::HandleFriendlyUnitClick(AActor* ClickedUnit)
{
        SelectUnit(ClickedUnit);
    
}

void AHumanPlayer::HandleEnemyUnitClick(AActor* ClickedEnemyUnit)
{
    if (bIsMyTurn && SelectedUnit)
    {
        AttackUnit();
    }
    else
    {
        // Optionally, handle clicking on an enemy unit when not ready to attack (e.g., show unit info)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enemy Clicked: %s"), *ClickedEnemyUnit->GetName()));
    }
}




void AHumanPlayer::SelectUnit(AActor* Unit)
{

    TArray<ATile*> ReachableTiles;

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode && GameMode->GameField)
    {
        GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
    }

    SelectedUnit = Unit;
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Selected: %s"), *Unit->GetName()));

    // Get reachable tiles

    //Try to cast to a Brawler
    AAW_Brawler* ClickedBrawler = Cast<AAW_Brawler>(SelectedUnit);
    if (ClickedBrawler)
    {
        
            ReachableTiles = ClickedBrawler->GetReachableTiles(ClickedBrawler->GetMovementRange());

            if (GameMode && GameMode->GameField)
            {
                GameMode->GameField->HighlightReachableTiles(ReachableTiles);
            }
        
    }
    else
    {
        //Try to cast to a Sniper
        AAW_Sniper* ClickedSniper = Cast<AAW_Sniper>(SelectedUnit);
        if (ClickedSniper)
        {
            
                ReachableTiles = ClickedSniper->GetReachableTiles(ClickedSniper->GetMovementRange());

                if (GameMode && GameMode->GameField)
                {
                    GameMode->GameField->HighlightReachableTiles(ReachableTiles);
                    bWaitingForMoveInput = true;
                    HandleTileClick(ClickedSniper->CurrentTile, ReachableTiles);
                }
            
        }
       
    }
}

void AHumanPlayer::HandleTileClick(ATile* CurrentTile, TArray<ATile*>& ReachableTiles) 
{
    // HandleTileClick waits for another input by the player
    // if the click is on the same tile it is on deselect the unit and goes back to OnClick()
    // else if is on a reachable tile moves or if on a enemy unit within the attack range shoots!

    GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Orange, TEXT("Handling Tile Click"));

    // Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
    FHitResult Hit = FHitResult(ForceInit);

    // GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    ATile* ClickedTile = Cast<ATile>(Hit.GetActor()); // Get the tile from the hit result

    if (bWaitingForMoveInput)
    {
        if (SelectedUnit)
        {
            if (ClickedTile && ClickedTile == CurrentTile) // Check if the clicked tile is the unit's current tile
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Deselecting unit and Attacking"));
                AttackUnit(); // Call AttackUnit() to handle attack logic
                SelectedUnit = nullptr; // Deselect the unit
            }
            else if (ClickedTile && ReachableTiles.Contains(ClickedTile)) // Check if the clicked tile is reachable
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Moving unit"));
               
                MoveUnit(CurrentTile, ClickedTile);

                if (GameMode && GameMode->GameField)
                {
                    GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
                }
                SelectedUnit = nullptr; // Deselect unit after move
            }
            else
            {
                // Handle click on a tile that's not reachable (e.g., show a message)
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Tile not reachable"));
            }
        }
        else
        {
            // Handle tile click when no unit is selected (e.g., show tile info?)
            if (ClickedTile)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Tile Clicked: %s"), *ClickedTile->GetName()));
            }
        }
    }
}






void AHumanPlayer::MoveUnit(ATile* CurrentTile, ATile* TargetTile)
{
    if (!SelectedUnit)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No unit selected!"));
        return;
    }

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameMode or GameField is invalid!"));
        return;
    }

    ATile* StartTile = CurrentTile;

    // 1. Find the path using AGameField::FindPath
    TArray<ATile*> Path = GameMode->GameField->FindPath(StartTile, TargetTile);

    if (Path.Num() == 0)
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No path found!"));
        return;
    }

    // 2. Initiate and handle the movement along the path
    if (Path.Num() > 0 && SelectedUnit)
    {
        CurrentPath = Path;
        CurrentPathIndex = 0;

       // GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &AHumanPlayer::MoveUnit, MovementSpeed, true);
       
        GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &AHumanPlayer::MoveUnit_Tick, MovementSpeed, true);
    }
    else
    {
        GetWorldTimerManager().ClearTimer(MovementTimerHandle);
        CurrentPath.Empty();
        CurrentPathIndex = 0;
    }
}




void AHumanPlayer::MoveUnit_Tick()
{
    if (CurrentPathIndex >= CurrentPath.Num() || !SelectedUnit)
    {
        GetWorldTimerManager().ClearTimer(MovementTimerHandle);
        CurrentPath.Empty();
        CurrentPathIndex = 0;
        return; // Reached the end or unit was deselected
    }

    ATile* NextTile = CurrentPath[CurrentPathIndex];
    FVector TargetLocation = NextTile->GetActorLocation();

    // Smoothly move the unit towards the target tile's location
    FVector CurrentLocation = SelectedUnit->GetActorLocation();
    FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), MovementSpeed * GetWorld()->GetDeltaSeconds());
    SelectedUnit->SetActorLocation(NewLocation);

    // Check if the unit has reached the tile
    if (FVector::DistSquared(SelectedUnit->GetActorLocation(), TargetLocation) < 1.0f) // Using a small tolerance
    {
        

        CurrentPathIndex++; // Move to the next tile in the path
    }
}

void AHumanPlayer::AttackUnit()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALL'ATTACCOOOOO!!!"));
}





