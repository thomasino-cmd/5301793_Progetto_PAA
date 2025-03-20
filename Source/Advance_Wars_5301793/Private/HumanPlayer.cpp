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

#include "CoreMinimal.h"  // Assicura che `int32` sia riconosciuto
#include "TimerManager.h" // Per i timer
#include "functional"
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
    bIsMoving = false;



}

// Called when the game starts or when spawned
void AHumanPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Bind OnClick as the default action
    OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
    bWaitingForMoveInput = false;
    SelectedUnitForMovement = nullptr;

   // CurrentMovementIndex = 0;
    
    
}




void AHumanPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    if (bIsMoving)
    {
        if (CurrentPathIndex < MovementPath.Num())
        {
            ATile* NextTile = MovementPath[CurrentPathIndex];
            FVector TargetLocation = NextTile->GetActorLocation() + FVector(0.0f, 0.0f, 15.0f);

            // Calcola la direzione e la distanza.
            FVector Direction = (TargetLocation - SelectedUnitForMovement->GetActorLocation()).GetSafeNormal();
            float DistanceToTarget = FVector::Distance(SelectedUnitForMovement->GetActorLocation(), TargetLocation);

            // Calcola quanto muoversi in questo frame.
            float FrameMovement = MoveSpeed * DeltaTime;

            // --- Correzione Movimento Diagonale ---
            float DeltaX = FMath::Abs(TargetLocation.X - SelectedUnitForMovement->GetActorLocation().X);
            float DeltaY = FMath::Abs(TargetLocation.Y - SelectedUnitForMovement->GetActorLocation().Y);

            if (DeltaX > DeltaY)
            {
                Direction.Y = 0.0f;
            }
            else
            {
                Direction.X = 0.0f;
            }
            Direction = Direction.GetSafeNormal();


            // Se siamo abbastanza vicini, spostati direttamente e passa alla prossima tile.
            if (FrameMovement >= DistanceToTarget)
            {
                SelectedUnitForMovement->SetActorLocation(TargetLocation);
                MovingCurrentTile->SetTileStatus(PlayerId, ETileStatus::EMPTY);
                NextTile->SetTileStatus(PlayerId, ETileStatus::OCCUPIED);
                UE_LOG(LogTemp, Log, TEXT("Moved unit to tile: %s"), *NextTile->GetName());

                // AGGIORNAMENTO TILE DEL SOLDATO (tramite interfaccia)
                IAW_BaseSoldier* Soldier = Cast<IAW_BaseSoldier>(SelectedUnitForMovement); //Cast all'interfaccia
                if (Soldier)
                {
                    Soldier->SetTileIsOnNow(NextTile);
                }
                else
                {
                    UE_LOG(LogTemp, Error, TEXT("Failed to cast SelectedUnitForMovement to IAW_BaseSoldier!"));
                }


                MovingCurrentTile = NextTile;
                CurrentPathIndex++;
                UE_LOG(LogTemp, Log, TEXT("Moving to next tile in path.  Index: %d"), CurrentPathIndex);



            }
            else // Altrimenti, muoviti di un passo.
            {
                SelectedUnitForMovement->SetActorLocation(SelectedUnitForMovement->GetActorLocation() + Direction * FrameMovement);
            }
        }
        else //Movimento terminato
        {

            if (MovementPath.Num() > 0)
            {
                IAW_BaseSoldier* Soldier = Cast<IAW_BaseSoldier>(SelectedUnitForMovement); //Cast all'interfaccia
                if (Soldier && Soldier->GetTileIsOnNow() != MovementPath.Last())
                {
                    if (Soldier->GetTileIsOnNow())
                    {
                        Soldier->GetTileIsOnNow()->SetTileStatus(PlayerId, ETileStatus::EMPTY);
                    }
                    MovementPath.Last()->SetTileStatus(PlayerId, ETileStatus::OCCUPIED);
                    Soldier->SetTileIsOnNow(MovementPath.Last());
                    UE_LOG(LogTemp, Log, TEXT("Movement completed. Final tile occupied."));
                }
            }



            UE_LOG(LogTemp, Log, TEXT("Movement completed."));
            bIsMoving = false;
            SelectedUnitForMovement->SetActorRotation(FRotator::ZeroRotator);

            MovingCurrentTile = nullptr;
            // MovingTargetTile = nullptr; // Non necessario
            MovementPath.Empty();
            SelectedUnitForMovement = nullptr;

        }
    }
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




void AHumanPlayer::AttackUnit()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("ALL'ATTACCOOOOO!!!"));
}



void AHumanPlayer::HandleFriendlyUnitClick(AActor* ClickedUnit)
{
        SelectUnit(ClickedUnit);
    
}

void AHumanPlayer::HandleEnemyUnitClick(AActor* ClickedEnemyUnit)
{
    //in realtà qui dovrò fare come per handletileclick però TODO LATER
    if (bIsMyTurn && SelectedUnitForMovement)
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
    if (!Unit) return;

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField) return;

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Selected: %s"), *Unit->GetName()));

    // Controlliamo se l'unità selezionata è un Brawler
    AAW_Brawler* ClickedBrawler = Cast<AAW_Brawler>(Unit);
    if (ClickedBrawler)
    {
        // Otteniamo le tile raggiungibili
        ClickedBrawler->TilesCanReach = ClickedBrawler->GetReachableTiles(ClickedBrawler->GetMovementRange());

        // Evidenziamo le tile raggiungibili
        GameMode->GameField->HighlightReachableTiles(ClickedBrawler->TilesCanReach);

        // Impostiamo l'input in attesa di movimento
        //SetWaitingForMoveInput(true, ClickedBrawler->TileIsOnNow, ClickedBrawler->TilesCanReach);
        SetWaitingForMoveInput(true, ClickedBrawler);
        
    }

    // Controlliamo se l'unità selezionata è uno Sniper
    AAW_Sniper* ClickedSniper = Cast<AAW_Sniper>(Unit);
    if (ClickedSniper)
    {
        // Otteniamo le tile raggiungibili
        ClickedSniper->TilesCanReach = ClickedSniper->GetReachableTiles(ClickedSniper->GetMovementRange());

        // Evidenziamo le tile raggiungibili
        GameMode->GameField->HighlightReachableTiles(ClickedSniper->TilesCanReach);

        // Impostiamo l'input in attesa di movimento
        //SetWaitingForMoveInput(true, ClickedSniper->TileIsOnNow, ClickedSniper->TilesCanReach);
        SetWaitingForMoveInput(true, ClickedSniper);
        
    
    }
}





void AHumanPlayer::SetWaitingForMoveInput(bool bWaiting, AActor* Unit)
{
    bWaitingForMoveInput = bWaiting;

    if (bWaiting)
    {
        SelectedUnitForMovement = Unit;  // Memorizza l'unità da muovere
        OnClickAction.BindUObject(this, &AHumanPlayer::HandleTileClick);
    }
    else
    {
        //SelectedUnitForMovement = nullptr;  // Resetta quando non stiamo aspettando input
        OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
    }
}



void AHumanPlayer::HandleTileClick()
{
    if (!SelectedUnitForMovement) return;

    FHitResult Hit = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (Hit.bBlockingHit && GameMode && GameMode->GameField)
    {
        FVector HitLocation = Hit.Location;
        FVector RelativeLocation = GameMode->GameField->GetActorTransform().InverseTransformPosition(HitLocation);
        FVector2D GridPosition2D = GameMode->GameField->GetXYPositionByRelativeLocation(RelativeLocation);

        ATile* ClickedTile = GameMode->GameField->GetTile(GridPosition2D.X, GridPosition2D.Y);
        if (!ClickedTile) return;

        AAW_Brawler* SelectedBrawler = Cast<AAW_Brawler>(SelectedUnitForMovement);
        AAW_Sniper* SelectedSniper = Cast<AAW_Sniper>(SelectedUnitForMovement);

        TArray<ATile*> ReachableTiles;
        ATile* CurrentTile = nullptr;

        if (SelectedBrawler)
        {
            ReachableTiles = SelectedBrawler->TilesCanReach;
            CurrentTile = SelectedBrawler->TileIsOnNow;
        }
        else if (SelectedSniper)
        {
            ReachableTiles = SelectedSniper->TilesCanReach;
            CurrentTile = SelectedSniper->TileIsOnNow;
        }

        if (!CurrentTile) return;

        if (ClickedTile == CurrentTile)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Deselecting unit and Attacking"));
            AttackUnit();
        }
        else if (ReachableTiles.Contains(ClickedTile))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Moving unit"));
            MoveUnit(CurrentTile, ClickedTile);

        }
        else if (ReachableTiles.Contains(ClickedTile))
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Tile not reachable"));
        }

        GameMode->GameField->ClearHighlightedTiles(ReachableTiles);
        SetWaitingForMoveInput(false, nullptr);
    }
}





//
//
//
//
//void AHumanPlayer::MoveUnit(ATile* CurrentTile, ATile* TargetTile)
//{
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//
//    if (!CurrentTile || !TargetTile || !SelectedUnitForMovement)
//    {
//        UE_LOG(LogTemp, Error, TEXT("MoveUnit: Parametri non validi"));
//        return;
//    }
//
//    TArray<ATile*> Path = GameMode->GameField->FindPath(CurrentTile, TargetTile);
//    if (Path.Num() <= 1)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("MoveUnit: Nessun movimento necessario"));
//        return;
//    }
//
//    float MoveDuration = 0.5f;
//
//    CurrentTile->SetTileStatus(PlayerId, ETileStatus::EMPTY);
//    UE_LOG(LogTemp, Warning, TEXT("MoveUnit: Tile di partenza liberata"));
//
//    int32 PathLength = Path.Num();
//    UE_LOG(LogTemp, Warning, TEXT("MoveUnit: Percorso calcolato con %d step"), PathLength);
//
//    // Now we store the path and begin movement
//    CurrentMovementIndex = 1;  // Start with step 1
//    MovementPath = Path;
//    MoveUnitToNextStep(MoveDuration);  // Start moving
//
//}
//
//void AHumanPlayer::MoveUnitToNextStep(float MoveDuration)
//{
//    if (!SelectedUnitForMovement || CurrentMovementIndex >= MovementPath.Num())
//    {
//        UE_LOG(LogTemp, Warning, TEXT("MoveUnit: Fine percorso o unità nulla. Indice: %d"), CurrentMovementIndex);
//        return;
//    }
//
//    // Get the next tile and its location
//    ATile* NextTile = MovementPath[CurrentMovementIndex];
//    FVector TargetLocation = NextTile->GetActorLocation();
//
//    UE_LOG(LogTemp, Warning, TEXT("MoveStep: Spostamento verso tile %d (X: %f, Y: %f, Z: %f)"), CurrentMovementIndex, TargetLocation.X, TargetLocation.Y, TargetLocation.Z);
//
//    SelectedUnitForMovement->SetActorLocationAndRotation(TargetLocation + FVector(0, 0, 2), FRotator::ZeroRotator);
//    UE_LOG(LogTemp, Warning, TEXT("MoveStep: Posizione aggiornata a %s"), *TargetLocation.ToString());
//
//    // Move to the next step
//    CurrentMovementIndex++;
//
//    if (CurrentMovementIndex < MovementPath.Num())
//    {
//        // Set the timer for the next move
//        FTimerHandle TimerHandle;
//        this->GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateUObject(this, &AHumanPlayer::MoveUnitToNextStep, MoveDuration), MoveDuration, false);
//    }
//    else
//    {
//        // If the unit has reached the destination, update the final tile status
//        UE_LOG(LogTemp, Warning, TEXT("MoveUnit: Arrivato alla destinazione"));
//        MovementPath.Last()->SetTileStatus(PlayerId, ETileStatus::OCCUPIED);
//    }
//}
//



//questa mi servirà dopo     AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());


void AHumanPlayer::MoveUnit(ATile* CurrentTile, ATile* TargetTile)
{
    if (!SelectedUnitForMovement || !CurrentTile || !TargetTile || CurrentTile == TargetTile)
    {
        return;
    }
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    MovementPath = GameMode->GameField->FindPath(CurrentTile, TargetTile);

    if (MovementPath.Num() <= 1)
    {
        return;
    }

    //Inizializza le variabili per il movimento
    MovingCurrentTile = CurrentTile;
    MovingTargetTile = TargetTile; //salviamo il target per usarlo alla fine
    MoveSpeed = 200.0f;
    bIsMoving = true; // Inizia il movimento
    CurrentPathIndex = 1; // Inizia dalla prima tile del percorso (dopo quella di partenza)

    AttackUnit();
}
