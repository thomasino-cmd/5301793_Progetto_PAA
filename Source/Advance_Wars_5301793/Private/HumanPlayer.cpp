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
    bIsMyTurn = false; 
   



}

// Called when the game starts or when spawned
void AHumanPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Bind OnClick as the default action
    OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
    bWaitingForMoveInput = false;
    SelectedUnitForMovement = nullptr;


    bBrawlerMovedThisTurn = false;
    bSniperMovedThisTurn = false;

   // CurrentMovementIndex = 0;
    
    
}




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



    bBrawlerMovedThisTurn = false;  // Reset flags at the start of the turn
    bSniperMovedThisTurn = false;

    
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
                    else   //else niente deve cliccare per forza prima su una sua anche per attacare senza muoversi 
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
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode is null in HandleFriendlyUnitClick"));
        return; // Exit if GameMode is invalid
    }

    // Get the human player's units from GameMode
    TArray<AActor*> HumanUnits = GameMode->GetCurrentPlayerUnits(PlayerId);

    // Check if the clicked unit is in the human player's units array
    if (HumanUnits.Contains(ClickedUnit))
    {
        SelectUnit(ClickedUnit);
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Clicked unit is not a friendly unit"));
        // Optionally, you could add some other feedback here, like a sound or visual cue
    }
}



void AHumanPlayer::HandleEnemyUnitClick(AActor* ClickedEnemyUnit)
{
    //in realtà qui dovrò fare come per handletileclick però TODO LATER
    if (bIsMyTurn && SelectedUnitForMovement)
    {
        
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
        if (bBrawlerMovedThisTurn)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CANT MOVE TWICE IN THE SAME TURN : %s"), *Unit->GetName()));
            return;
        }
        // Otteniamo le tile raggiungibili
        ClickedBrawler->TilesCanReach = ClickedBrawler->GetReachableTiles(ClickedBrawler->GetMovementRange());

        // Evidenziamo le tile raggiungibili
        GameMode->GameField->HighlightReachableTiles(ClickedBrawler->TilesCanReach);

        // Impostiamo l'input in attesa di movimento
        //SetWaitingForMoveInput(true, ClickedBrawler->TileIsOnNow, ClickedBrawler->TilesCanReach);
        SetWaitingForMoveInput(true, Unit); // Use Unit directly
    }

    // Controlliamo se l'unità selezionata è uno Sniper
    AAW_Sniper* ClickedSniper = Cast<AAW_Sniper>(Unit);
    if (ClickedSniper)
    {
        if (bSniperMovedThisTurn)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CANT MOVE TWICE IN THE SAME TURN : %s"), *Unit->GetName()));
            return;
        }
        // Otteniamo le tile raggiungibili
        ClickedSniper->TilesCanReach = ClickedSniper->GetReachableTiles(ClickedSniper->GetMovementRange());

        // Evidenziamo le tile raggiungibili
        GameMode->GameField->HighlightReachableTiles(ClickedSniper->TilesCanReach);

        // Impostiamo l'input in attesa di movimento
        //SetWaitingForMoveInput(true, ClickedSniper->TileIsOnNow, ClickedSniper->TilesCanReach);

        SetWaitingForMoveInput(true, Unit);  // Use Unit directly
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

    FHitResult HitMove = FHitResult(ForceInit);
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, HitMove);
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (HitMove.bBlockingHit && GameMode && GameMode->GameField)
    {
        ATile* ClickedTile = Cast<ATile>(HitMove.GetActor());
        if (!ClickedTile) return;

        AAW_Brawler* SelectedBrawler = Cast<AAW_Brawler>(SelectedUnitForMovement);
        AAW_Sniper* SelectedSniper = Cast<AAW_Sniper>(SelectedUnitForMovement);

        if (SelectedBrawler)
        {
            if (!bBrawlerMovedThisTurn && SelectedBrawler->TilesCanReach.Contains(ClickedTile))
            {
                SelectedBrawler->MoveUnit(ClickedTile);

                GameMode->GameField->ClearHighlightedTiles(SelectedBrawler->TilesCanReach);
                SetWaitingForMoveInput(false, nullptr);

                // Attendi che il Brawler abbia finito di muoversi
        
                GetWorldTimerManager().SetTimer(TimerBrawler, [this, SelectedBrawler, GameMode]() {
                    if (!SelectedBrawler->bIsMoving)
                    {
                        bBrawlerMovedThisTurn = true;
                        GetWorldTimerManager().ClearTimer(TimerBrawler);
                        CheckAndEndTurn(); // Funzione per controllare e terminare il turno
                    }
                    else
                    {
                        // Continua a controllare se il Brawler ha finito di muoversi
                       /* FTimerHandle RetryHandle;
                        GetWorldTimerManager().SetTimer(RetryHandle, FTimerDelegate::CreateLambda([this, SelectedBrawler, GameMode]() {*/
                          // CheckAndEndTurn();

                        /*    }), 0.1f, false);*/
                    }
                    }, 0.1f, true);
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Brawler cannot move again or tile not reachable"));
            }
        }
        else if (SelectedSniper)
        {
            if (!bSniperMovedThisTurn && SelectedSniper->TilesCanReach.Contains(ClickedTile))
            {
                SelectedSniper->MoveUnit(ClickedTile);
                GameMode->GameField->ClearHighlightedTiles(SelectedSniper->TilesCanReach);
                SetWaitingForMoveInput(false, nullptr);

                // Attendi che lo Sniper abbia finito di muoversi
                
                GetWorldTimerManager().SetTimer(TimerSniper, [this, SelectedSniper, GameMode]() {
                    if (!SelectedSniper->bIsMoving)
                    {
                        bSniperMovedThisTurn = true;
                        GetWorldTimerManager().ClearTimer(TimerSniper);
                        CheckAndEndTurn(); // Funzione per controllare e terminare il turno
                    }
                    else
                    {
                        // Continua a controllare se lo Sniper ha finito di muoversi
                      /*  FTimerHandle RetryHandle;
                        GetWorldTimerManager().SetTimer(RetryHandle, FTimerDelegate::CreateLambda([this, SelectedSniper, GameMode]() {*/
                           // CheckAndEndTurn();

                         /*   }), 0.1f, false);*/
                    }
                    }, 0.1f, true);
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sniper cannot move again or tile not reachable"));
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Selected unit is not a Brawler or Sniper!"));
        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Invalid tile click"));
    }
}










void AHumanPlayer::CheckAndEndTurn()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (bBrawlerMovedThisTurn && bSniperMovedThisTurn)
    {
        if (GameMode)
        {
            bIsMyTurn = false;
            GameMode->EndTurn();
        }
    }
}