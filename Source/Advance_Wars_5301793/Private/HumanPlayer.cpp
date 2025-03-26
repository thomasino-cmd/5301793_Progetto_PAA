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
    bWaitingForAttackInput = false;

    SelectedUnitForAttack = nullptr;
    SelectedUnitForMovement = nullptr;


    bBrawlerMovedThisTurn = false;
    bSniperMovedThisTurn = false;

   // CurrentMovementIndex = 0;
    
    bSniperAttackedThisTurn = false;
    bBrawlerAttackedThisTurn = false;
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

    SelectedUnitForAttack = nullptr;
    SelectedUnitForMovement = nullptr;

    bBrawlerMovedThisTurn = false;  // Reset flags at the start of the turn
    bSniperMovedThisTurn = false;

    bSniperAttackedThisTurn = false;
    bBrawlerAttackedThisTurn = false;

    bWaitingForMoveInput = false;
    bWaitingForAttackInput = false;

    OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);

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








void AHumanPlayer::HandleFriendlyUnitClick(AActor* ClickedUnit)     //this is executed only if i previously clicked on a unit among mines
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode is null in HandleFriendlyUnitClick"));
        return;
    }

    // Get the human player's units from GameMode
    TArray<AActor*> HumanUnits = GameMode->GetCurrentPlayerUnits(PlayerId);

    // Check if the clicked unit is in the human player's units array
    if (HumanUnits.Contains(ClickedUnit))
    {
        // Verifica se ci sono Brawler e Sniper
        bool HasBrawler = false;
        bool HasSniper = false;

        for (AActor* Unit : HumanUnits)
        {
            if (Cast<AAW_Brawler>(Unit))
            {
                HasBrawler = true;
            }
            else if (Cast<AAW_Sniper>(Unit))
            {
                HasSniper = true;
            }
        }

        // Chiama SelectUnit solo se ci sono Brawler o Sniper
        if (HasBrawler || HasSniper)
        {
            SelectUnit(ClickedUnit);
        }
        else
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No Brawler or Sniper available. Ending turn."));

            // Set the flags to true to allow turn to end
            if (!HasBrawler)
            {
                bBrawlerMovedThisTurn = true;
                bBrawlerAttackedThisTurn = true;
            }
            if (!HasSniper)
            {
                bSniperMovedThisTurn = true;
                bSniperAttackedThisTurn = true;
            }
            if (!HasBrawler && !HasSniper)
            {
                //game should end because it has lost 
            }

        }
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Clicked unit is not a friendly unit"));
        
    }
}






void AHumanPlayer::HandleEnemyUnitClick()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField) return;

    if (!bWaitingForAttackInput) {
        return;
    }

    if (bIsMyTurn && SelectedUnitForAttack)
    {
        // 1. Ottieni l'attore cliccato
        FHitResult Hit = FHitResult(ForceInit);
        GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
        AActor* ClickedActor = Hit.GetActor();

        if (!ClickedActor)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Nessun attore selezionato per l'attacco."));
            return;
        }

        ATile* ClickedTile = Cast<ATile>(ClickedActor);
        IAW_BaseSoldier* ClickedSoldier = Cast<IAW_BaseSoldier>(ClickedActor);

        // 2. Determina se è stata cliccata una tile o un soldato
        if (ClickedTile)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Nessun nemico selezionato per l'attacco."));
            
        }
        else if (ClickedSoldier)
        {
            ClickedTile = ClickedSoldier->GetTileIsOnNow();
            // È stato cliccato un soldato
        }
        else
        {
            // Cliccato qualcos'altro (non gestito)
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Cliccato un attore non valido per l'attacco."));
            return;
        }


        bWaitingForAttackInput = false;

        AAW_Brawler* AttackingBrawler = Cast<AAW_Brawler>(SelectedUnitForAttack);
        AAW_Sniper* AttackingSniper = Cast<AAW_Sniper>(SelectedUnitForAttack);

        TArray<ATile*> AttackableTiles;

        if (AttackingBrawler)
        {
            AttackableTiles = AttackingBrawler->GetAttackableTiles();

            // Controllo skip attacco: tile cliccata = tile dell'attaccante
            if (AttackingBrawler->GetTileIsOnNow() == ClickedTile)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("skipping attack even if possible ."));
                GameMode->GameField->ClearHighlightedAttackTiles(AttackableTiles);
                bBrawlerAttackedThisTurn = true;
                //CheckAndEndTurn();
                
            }
            else if (AttackableTiles.Contains(ClickedTile))                     // Controllo che la tile cliccata sia fra quelle a portata di tiro
            {
                GameMode->GameField->ClearHighlightedAttackTiles(AttackableTiles);
                AttackingBrawler->Shoot(ClickedTile); // Passa la tile cliccata
                bBrawlerAttackedThisTurn = true;

            }
            
        }
        else if (AttackingSniper)
        {
            AttackableTiles = AttackingSniper->GetAttackableTiles();

            // Controllo skip attacco: tile cliccata = tile dell'attaccante
            if (AttackingSniper->GetTileIsOnNow() == ClickedTile)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("skipping attack even if possible ."));
                GameMode->GameField->ClearHighlightedAttackTiles(AttackableTiles);
                bSniperAttackedThisTurn = true;
                //CheckAndEndTurn();
                
            }                                
            else if (AttackableTiles.Contains(ClickedTile))             // Controllo che la tile cliccata sia fra quelle a portata di tiro
            {
                GameMode->GameField->ClearHighlightedAttackTiles(AttackableTiles);
                AttackingSniper->Shoot(ClickedTile); // Passa la tile cliccata
                bSniperAttackedThisTurn = true;

            }
          
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("HandleEnemyUnitClick: l'unità selezionata non è un Brawler o uno Sniper!"));
            return;
        }


        // 7. Resetta il binding dell’input e passa al turno successivo
        //OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
        CheckAndEndTurn();
    }
    else
    {
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red,
            TEXT("Attacco non valido: non sei in modalità attacco o SelectedUnitForAttack è nullo."));
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
        AActor* HitActor = HitMove.GetActor(); // Ottieni l'attore colpito

        AAW_Brawler* SelectedBrawler = Cast<AAW_Brawler>(SelectedUnitForMovement);
        AAW_Sniper* SelectedSniper = Cast<AAW_Sniper>(SelectedUnitForMovement);

        if (SelectedBrawler)
        {
            if (HitActor == SelectedBrawler) // Se clicchi sul Brawler stesso
            {
                //ClickedTile = SelectedBrawler->GetTileIsOnNow(); // Ottieni la tile su cui si trova
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Skipping movement and going into attack phase"));
                GameMode->GameField->ClearHighlightedTiles(SelectedBrawler->TilesCanReach);
                bBrawlerMovedThisTurn = true;
                AttackPhase(SelectedBrawler);
            }
            else if (ClickedTile && !bBrawlerMovedThisTurn && SelectedBrawler->TilesCanReach.Contains(ClickedTile))
            {
                SelectedBrawler->MoveUnit(ClickedTile);
                GameMode->GameField->ClearHighlightedTiles(SelectedBrawler->TilesCanReach);
                //SetWaitingForMoveInput(false, nullptr);

                GetWorldTimerManager().SetTimer(TimerBrawler, [this, SelectedBrawler, GameMode]() {
                    if (!SelectedBrawler->bIsMoving)
                    {
                        bBrawlerMovedThisTurn = true;
                        GetWorldTimerManager().ClearTimer(TimerBrawler);
                        AttackPhase(SelectedBrawler);
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
            if (HitActor == SelectedSniper) // Se clicchi sullo Sniper stesso
            {
               // ClickedTile = SelectedSniper->GetTileIsOnNow(); // Ottieni la tile su cui si trova
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Skipping movement and going into attack phase"));
                GameMode->GameField->ClearHighlightedTiles(SelectedSniper->TilesCanReach);
                bSniperMovedThisTurn = true;
                AttackPhase(SelectedSniper);
            }
            else if (ClickedTile && !bSniperMovedThisTurn && SelectedSniper->TilesCanReach.Contains(ClickedTile))
            {
                SelectedSniper->MoveUnit(ClickedTile);
                GameMode->GameField->ClearHighlightedTiles(SelectedSniper->TilesCanReach);
                //SetWaitingForMoveInput(false, nullptr);

                GetWorldTimerManager().SetTimer(TimerSniper, [this, SelectedSniper, GameMode]() {
                    if (!SelectedSniper->bIsMoving)
                    {
                        bSniperMovedThisTurn = true;
                        GetWorldTimerManager().ClearTimer(TimerSniper);
                        AttackPhase(SelectedSniper);
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




void AHumanPlayer::AttackPhase(IAW_BaseSoldier* SelectedUnit)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (!SelectedUnit)
        return;

    TArray<ATile*> AttackableTiles;

    // Esegui il cast per ottenere il tipo concreto
    if (AAW_Brawler* BrawlerUnit = Cast<AAW_Brawler>(SelectedUnit))
    {
        // Ottieni le tile a portata specifiche per il Brawler
        AttackableTiles = BrawlerUnit->GetAttackableTiles();
        SelectedUnitForAttack = BrawlerUnit;

        // Controllo se ci sono nemici nelle tile attaccabili
        bool bEnemyFound = false;
        for (ATile* Tile : AttackableTiles)
        {
            AActor* SoldierOnTileActor = Tile->GetUnit();
            if (SoldierOnTileActor)
            {
                IAW_BaseSoldier* SoldierOnTile = Cast<IAW_BaseSoldier>(SoldierOnTileActor);
                if (SoldierOnTile && SoldierOnTile->GetOwnerPlayerId() != PlayerId)
                {
                    bEnemyFound = true;
                    break;
                }
            }
        }

        // Se non ci sono nemici, skippa la fase di attacco
        if (!bEnemyFound)
        {
            bBrawlerAttackedThisTurn = true;
            CheckAndEndTurn();
            return;
        }

    }
    else if (AAW_Sniper* SniperUnit = Cast<AAW_Sniper>(SelectedUnit))
    {
        // Ottieni le tile a portata specifiche per lo Sniper
        AttackableTiles = SniperUnit->GetAttackableTiles();
        SelectedUnitForAttack = SniperUnit;

        // Controllo se ci sono nemici nelle tile attaccabili
        bool bEnemyFound = false;
        for (ATile* Tile : AttackableTiles)
        {
            AActor* SoldierOnTileActor = Tile->GetUnit();
            if (SoldierOnTileActor)
            {
                IAW_BaseSoldier* SoldierOnTile = Cast<IAW_BaseSoldier>(SoldierOnTileActor);
                if (SoldierOnTile && SoldierOnTile->GetOwnerPlayerId() != PlayerId)
                {
                    bEnemyFound = true;
                    break;
                }
            }

        }

        // Se non ci sono nemici, skippa la fase di attacco
        if (!bEnemyFound)
        {
            bSniperAttackedThisTurn = true;
            CheckAndEndTurn();
            return;
        }


    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AttackPhase: Unit cast fallito."));
        bBrawlerAttackedThisTurn = true;
        bSniperAttackedThisTurn = true;
       // OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
        CheckAndEndTurn();
        return;
    }

    

    // Esegui l'highlight e l'attivazione del binding per il click di attacco
    GameMode->GameField->HighlightAttackTiles(AttackableTiles, SelectedUnit->OwnerPlayerId);
    bWaitingForAttackInput = true;
    // Imposta il binding per la gestione del click durante la fase d'attacco
    OnClickAction.BindUObject(this, &AHumanPlayer::HandleEnemyUnitClick);
}







void AHumanPlayer::CheckAndEndTurn()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (bBrawlerMovedThisTurn && bSniperMovedThisTurn)
    {
        if(bBrawlerAttackedThisTurn && bSniperAttackedThisTurn)
		{
			bIsMyTurn = false;
            SelectedUnitForAttack = nullptr;
            SelectedUnitForMovement = nullptr;
            bWaitingForAttackInput = false;
            bWaitingForMoveInput = false;
            //OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);
			GameMode->EndTurn();
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("You must attack with all your units"));
		}
        
    }
    else
    {
        OnClickAction.BindUObject(this, &AHumanPlayer::OnClick);

    }
}