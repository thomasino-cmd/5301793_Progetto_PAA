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
#include "Kismet/KismetSystemLibrary.h"
#include "TimerManager.h"

#include "GameFramework/Actor.h"
#include "Kismet/KismetMathLibrary.h"

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
    MovementSpeed = 100.0f;
    CurrentActorTile = nullptr;

}

// Called when the game starts or when spawned
void AHumanPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Bind OnClick as the default action
    OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
    bWaitingForMoveInput = false;
    
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

void AHumanPlayer::SetWaitingForMoveInput(bool bWaiting, ATile* SoldierTileForNow, const TArray<ATile*>& Tiles)
{
    bWaitingForMoveInput = bWaiting;
    if (bWaiting)
    {
        // Bind the delegate to HandleTileClick
        OnClickAction.BindUObject(this, &AHumanPlayer::HandleTileClick);
        SetCurrentTile(SoldierTileForNow);
        SetReachableTiles(Tiles);       //these are the actual reachable tiles, take care.
    }
    else
    {
        // Bind the delegate to OnClick
        OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
        SetCurrentTile(nullptr);   // Reset CurrentTile
        SetReachableTiles(TArray<ATile*>()); // Reset ReachableTiles
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


//
//
//void AHumanPlayer::SelectUnit(AActor* Unit)
//{
//
//    TArray<ATile*> ReachableTiles;
//
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//    if (GameMode && GameMode->GameField)
//    {
//        GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
//    }
//
//    SelectedUnit = Unit;
//    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Selected: %s"), *Unit->GetName()));
//
//    // Get reachable tiles
//
//    //Try to cast to a Brawler
//    AAW_Brawler* ClickedBrawler = Cast<AAW_Brawler>(SelectedUnit);
//    if (ClickedBrawler)
//    {
//        
//            ReachableTiles = ClickedBrawler->GetReachableTiles(ClickedBrawler->GetMovementRange());
//
//            if (GameMode && GameMode->GameField)
//            {
//                GameMode->GameField->HighlightReachableTiles(ReachableTiles);
//
//                //SetWaitingForMoveInput(true, ClickedBrawler->CurrentTile,  ReachableTiles);
//                SetWaitingForMoveInput(true);
//
//                HandleTileClick(ClickedBrawler->CurrentTile, ReachableTiles);
//            }
//        
//    }
//    else
//    {
//        //Try to cast to a Sniper
//        AAW_Sniper* ClickedSniper = Cast<AAW_Sniper>(SelectedUnit);
//        if (ClickedSniper)
//        {
//            
//                ReachableTiles = ClickedSniper->GetReachableTiles(ClickedSniper->GetMovementRange());
//
//                if (GameMode && GameMode->GameField)
//                {
//                    GameMode->GameField->HighlightReachableTiles(ReachableTiles);
//                    
//                    //SetWaitingForMoveInput(true, ClickedSniper->CurrentTile, ReachableTiles);
//                    SetWaitingForMoveInput(true);
//
//                    HandleTileClick(ClickedSniper->CurrentTile, ReachableTiles);
//                }
//            
//        }
//       
//    }
//}
//
//void AHumanPlayer::HandleTileClick(ATile* CurrentTile, TArray<ATile*>& ReachableTiles) 
//{
//    // HandleTileClick waits for another input by the player
//    // if the click is on the same tile it is on deselect the unit and goes back to OnClick()
//    // else if is on a reachable tile moves or if on a enemy unit within the attack range shoots!
//
//    GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Orange, TEXT("Handling Tile Click"));
//
//    // Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
//    FHitResult Hit = FHitResult(ForceInit);
//
//    // GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
//    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//    ATile* ClickedTile = Cast<ATile>(Hit.GetActor()); // Get the tile from the hit result
//
//    if (bWaitingForMoveInput)
//    {
//        if (SelectedUnit)
//        {
//            if (ClickedTile && ClickedTile == CurrentTile) // Check if the clicked tile is the unit's current tile
//            {
//                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Deselecting unit and Attacking"));
//                AttackUnit(); // Call AttackUnit() to handle attack logic
//                SelectedUnit = nullptr; // Deselect the unit
//            }
//            else if (ClickedTile && ReachableTiles.Contains(ClickedTile)) // Check if the clicked tile is reachable
//            {
//                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Moving unit"));
//               
//                MoveUnit(CurrentTile, ClickedTile);
//
//                if (GameMode && GameMode->GameField)
//                {
//                    GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
//                }
//                SelectedUnit = nullptr; // Deselect unit after move
//            }
//            else
//            {
//                // Handle click on a tile that's not reachable (e.g., show a message)
//                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Tile not reachable"));
//            }
//        }
//        else
//        {
//            // Handle tile click when no unit is selected (e.g., show tile info?)
//            if (ClickedTile)
//            {
//                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Tile Clicked: %s"), *ClickedTile->GetName()));
//            }
//        }
//    }
//}
//



// NEW SELECT UNIT 
void AHumanPlayer::SelectUnit(AActor* Unit)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode && GameMode->GameField)
    {
        GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
    }

    SelectedUnit = Unit;
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Selected: %s"), *Unit->GetName()));

    // Get reachable tiles
    TArray<ATile*> ReachableTilesLocalArray; // Local scope variable

    //Try to cast to a Brawler
    AAW_Brawler* ClickedBrawler = Cast<AAW_Brawler>(SelectedUnit);
    if (ClickedBrawler)
    {
        ReachableTilesLocalArray = ClickedBrawler->GetReachableTiles(ClickedBrawler->GetMovementRange());

        if (GameMode && GameMode->GameField)
        {
            GameMode->GameField->HighlightReachableTiles(ReachableTilesLocalArray);

            //SetWaitingForMoveInput(true, ClickedBrawler->CurrentTile,  ReachableTiles);
            SetWaitingForMoveInput(true, ClickedBrawler->CurrentTile, ReachableTilesLocalArray);
        }
    }
    else
    {
        //Try to cast to a Sniper
        AAW_Sniper* ClickedSniper = Cast<AAW_Sniper>(SelectedUnit);
        if (ClickedSniper)
        {
            ReachableTilesLocalArray = ClickedSniper->GetReachableTiles(ClickedSniper->GetMovementRange());

            if (GameMode && GameMode->GameField)
            {
                GameMode->GameField->HighlightReachableTiles(ReachableTilesLocalArray);

                //SetWaitingForMoveInput(true, ClickedSniper->CurrentTile, ReachableTiles);
                SetWaitingForMoveInput(true, ClickedSniper->CurrentTile, ReachableTilesLocalArray);
            }
        }
    }
}



//NEW HANDLE TILE CLICK WITH NO ARGUMENTS
void AHumanPlayer::HandleTileClick()
{
    ATile* ClickedTile = nullptr;

    FHitResult Hit = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (Hit.bBlockingHit && GameMode && GameMode->GameField)
    {
        // Get the relative location of the hit point
        FVector HitLocation = Hit.Location;
        FVector RelativeLocation = GameMode->GameField->GetActorTransform().InverseTransformPosition(HitLocation);

        // Convert the relative location to grid position
        FVector2D GridPosition2D = GameMode->GameField->GetXYPositionByRelativeLocation(RelativeLocation);

        // Get the tile at the grid position
        ClickedTile = GameMode->GameField->GetTile(GridPosition2D.X, GridPosition2D.Y);
    }

    if (bWaitingForMoveInput)
    {
        if (SelectedUnit)
        {
            if (ClickedTile && ClickedTile == CurrentActorTile)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Deselecting unit and Attacking"));
                AttackUnit();
                SelectedUnit = nullptr;
                //CAZZATA NON MI MUOVO SetCurrentTile(nullptr);
                // ALTRA CAZZATA le tiles raggiungibili rimangono le stesse SetReachableTiles(TArray<ATile*>());
                SetWaitingForMoveInput(false, nullptr, TArray<ATile*>());
            }
            else if (ClickedTile && ReachableTiles.Contains(ClickedTile))
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Moving unit"));
                
                //SelectedUnit->SetActorLocationAndRotation(ClickedTile->GetActorLocation() + FVector(0, 0, 2), FRotator::ZeroRotator);
                
                
                MoveUnit(CurrentActorTile, ClickedTile);


                // devo togliere l'illuminazione alle tile raggiungibili , cambiarela nuova current tile verrà fatto al prossimo click per muoversi però annullarla male non fa 
                if (GameMode && GameMode->GameField)
                {
                    GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
                }
                //SelectedUnit = nullptr;
                
                SetCurrentTile(nullptr);
                SetReachableTiles(TArray<ATile*>());
                //devo forse chiamare il delegato per tornare su onclick 
                SetWaitingForMoveInput(false, nullptr, TArray<ATile*>());
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Tile not reachable"));
            }
        }
        else
        {
            if (ClickedTile)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, FString::Printf(TEXT("Tile Clicked: %s"), *ClickedTile->GetName()));
            }
        }
    }
}













//
//void AHumanPlayer::MoveUnit(ATile* CurrentTiles, ATile* TargetTile)
//{
//    if (!SelectedUnit)
//    {
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No unit selected!"));
//        return;
//    }
//
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//    if (!GameMode || !GameMode->GameField)
//    {
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("GameMode or GameField is invalid!"));
//        return;
//    }
//
//    ATile* StartTile = CurrentTiles;
//
//    // 1. Find the path using AGameField::FindPath
//    TArray<ATile*> Path = GameMode->GameField->FindPath(StartTile, TargetTile);
//
//    if (Path.Num() == 0)
//    {
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No path found!"));
//        return;
//    }
//
//    // 2. Initiate and handle the movement along the path
//    if (Path.Num() > 0 && SelectedUnit)
//    {
//        CurrentPath = Path;
//        CurrentPathIndex = 0;
//
//       
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Setting timer..."));
//        // Imposta l'intervallo di tempo (ad esempio, 0.02 secondi)
//        GetWorldTimerManager().SetTimer(MovementTimerHandle, this, &AHumanPlayer::MoveUnit_Tick, 0.02f, true);
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("Timer set."));
//    }
//    else
//    {
//        GetWorldTimerManager().ClearTimer(MovementTimerHandle);
//        CurrentPath.Empty();
//        CurrentPathIndex = 0;
//    }
//}




//void AHumanPlayer::MoveUnit_Tick()
//{
//    if (CurrentPathIndex >= CurrentPath.Num() || !SelectedUnit)
//    {
//        GetWorldTimerManager().ClearTimer(MovementTimerHandle);
//        CurrentPath.Empty();
//        CurrentPathIndex = 0;
//        return; // Reached the end or unit was deselected
//    }
//
//    ATile* NextTile = CurrentPath[CurrentPathIndex];
//    FVector TargetLocation = NextTile->GetActorLocation();
//
//    // Smoothly move the unit towards the target tile's location
//    //FVector CurrentLocation = SelectedUnit->GetActorLocation();
//    //FVector NewLocation = FMath::VInterpConstantTo(CurrentLocation, TargetLocation, GetWorld()->GetDeltaSeconds(), MovementSpeed);
//    //SelectedUnit->SetActorLocation(NewLocation);
//
//
//    SelectedUnit->SetActorLocationAndRotation(TargetLocation + FVector(0, 0, 2), FRotator::ZeroRotator);
//
//    // Check if the unit has reached the tile
//    if (FVector::DistSquared(SelectedUnit->GetActorLocation(), TargetLocation) < 2.25f) // Esempio: 1.5f * 1.5f
//    {
//        
//
//        CurrentPathIndex++; // Move to the next tile in the path
//    }
//}


void AHumanPlayer::MoveUnit(ATile* CurrTile, ATile* TargetTile)
{
    CurrTile = CurrentActorTile;
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (!SelectedUnit || !CurrTile || !TargetTile || !GameMode || !GameMode->GameField)
    {
        return;
    }

    TArray<ATile*> Path = GameMode->GameField->FindPath(CurrTile, TargetTile);
    if (Path.Num() <= 0)
    {
        return;
    }

    CurrTile->SetTileStatus(PlayerId, ETileStatus::EMPTY);

    FTimerHandle TimerHandle;
    float MoveSpeed = 255.0f;
    int32 PathIndex = 0;
    bool bMovementFinished = false;

    FTimerDelegate MoveDelegate;
    MoveDelegate.BindLambda([this, Path, MoveSpeed, CurrTile, TargetTile, &PathIndex, &TimerHandle, &bMovementFinished]()
        {
            if (PathIndex >= 0 && PathIndex < Path.Num())
            {
                FVector TargetLocation = Path[PathIndex]->GetActorLocation();
                FVector TargetLocationWithOffset = TargetLocation + FVector(0, 0, 2);
                FVector CurrentLocation = SelectedUnit->GetActorLocation();

                FVector Direction = (TargetLocationWithOffset - CurrentLocation).GetSafeNormal();

                // Determine the dominant direction
                float DeltaX = FMath::Abs(TargetLocationWithOffset.X - CurrentLocation.X);
                float DeltaY = FMath::Abs(TargetLocationWithOffset.Y - CurrentLocation.Y);

                if (DeltaX > DeltaY)
                {
                    // Move horizontally
                    Direction.Y = 0.0f;
                }
                else
                {
                    // Move vertically
                    Direction.X = 0.0f;
                }

                // Clamp the direction
                if (!Direction.IsNearlyZero())
                {
                    Direction = Direction.GetClampedToMaxSize(1.0f);
                }

                FVector NewLocation = CurrentLocation + Direction * MoveSpeed * GetWorld()->GetDeltaSeconds();

                SelectedUnit->SetActorLocationAndRotation(NewLocation, FRotator::ZeroRotator);

                float DistSq = FVector::DistSquared(SelectedUnit->GetActorLocation(), TargetLocationWithOffset);
                if (DistSq <= 0.01f)
                {
                    PathIndex++;
                }
            }
            else
            {
                if (GetWorldTimerManager().IsTimerActive(TimerHandle) && !bMovementFinished)
                {
                    bMovementFinished = true;
                    TargetTile->SetTileStatus(PlayerId, ETileStatus::OCCUPIED);
                    GetWorldTimerManager().ClearTimer(TimerHandle);

                    // Update CurrentActorTile after the movement
                    if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(SelectedUnit))
                    {
                        Brawler->CurrentTile = TargetTile;
                    }
                    else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(SelectedUnit))
                    {
                        Sniper->CurrentTile = TargetTile;
                    }
                    CurrentActorTile = TargetTile;
                }
            }
        });

    GetWorldTimerManager().SetTimer(TimerHandle, MoveDelegate, 0.01f, true);
}

void AHumanPlayer::MoveUnit_Tick()
{
    if (!SelectedUnit)
    {
        CurrentPath.Empty();
        CurrentPathIndex = 0;
        return;
    }

    if (CurrentPathIndex < CurrentPath.Num())
    {
        ATile* NextTile = CurrentPath[CurrentPathIndex];
        FVector TargetLocation = NextTile->GetActorLocation();

        // Move the unit to the next tile
        SelectedUnit->SetActorLocationAndRotation(TargetLocation + FVector(0, 0, 2), FRotator::ZeroRotator);

        CurrentPathIndex++;

        // Delay before moving to the next tile (using UKismetSystemLibrary::Delay)
        if (CurrentPathIndex < CurrentPath.Num())
        {
            FTimerHandle TimerHandle;
            GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]() {
                MoveUnit_Tick(); // Call MoveUnit_Tick again after the delay
                }, 1.0f, false); // Delay for 1 second
        }
        else
        {
            // Movement finished
            CurrentPath.Empty();
            CurrentPathIndex = 0;
        }
    }
    else
    {
        // Movement finished
        CurrentPath.Empty();
        CurrentPathIndex = 0;
    }
}

void AHumanPlayer::AttackUnit()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALL'ATTACCOOOOO!!!"));
}




// setters e getter per accedere alle variabili membro 

void AHumanPlayer::SetCurrentTile(ATile* Tile)
{
    CurrentActorTile = Tile;
}

void AHumanPlayer::SetReachableTiles(TArray<ATile*> Tiles)
{
    ReachableTiles = Tiles;
}


TArray<ATile*> AHumanPlayer::GetReachableTiles() const
{
    return ReachableTiles;
}