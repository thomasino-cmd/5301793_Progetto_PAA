

#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AW_Brawler.h"
#include "AW_Sniper.h"
#include "GameField.h" // Assicurati di includere GameField.h

#include "Engine/Engine.h"
#include "TimerManager.h"

#include "Tile.h"

AComputerPlayer::AComputerPlayer()
{
    PrimaryActorTick.bCanEverTick = true; 

    PlayerId = 1;
    SelectedBrawler = nullptr;
    SelectedSniper = nullptr;
    bBrawlerMoved = false;
    bSniperMoved = false;
    bBrawlerAttackedThisTurn = false;
    bSniperAttackedThisTurn = false;



    GameIstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
        //AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
}


// Funzione Tick, chiamata ad ogni frame
void AComputerPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

}


void AComputerPlayer::OnTurn()
{

    //if (bTurnEnded) return;

    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    // Imposta lo stato su Idle all'inizio del turno.
    //CurrentState = EComputerMoveState::Idle;

    if (GameMode)
    {
        if (GameMode->bIsPlacementPhaseOver == false)
        {
            PlaceUnit();
        }
        else if (GameMode->bIsPlacementPhaseOver == true)
        {
            // Controlla se l'AI ha già effettuato la sua mossa in questo turno.
            if (CurrentState == EComputerMoveState::Idle)
            {
                CurrentState = EComputerMoveState::Moving;
              //  GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AI: turno di MUOVERE"));
                MakeMove();
            }
            //Se lo stato non è idle non fare nulla, altrimenti MakeMove viene chiamata più volte.
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameMode!"));
    }
}



void AComputerPlayer::MakeMove()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (!GameMode)
    {
       // UE_LOG(LogTemp, Error, TEXT("AComputerPlayer::OnTurn - GameMode nullo"));
        return;
    }

    AIUnits = GameMode->GetCurrentPlayerUnits(PlayerId);

    if (AIUnits.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer: Nessuna unità AI disponibile. Termino turno."));
        GameMode->EndTurn();
        return;
    }

    // Reset dei flag per il movimento
    bBrawlerMoved = false;
    bSniperMoved = false;

    bBrawlerAttackedThisTurn = false;
    bSniperAttackedThisTurn = false;

    // Verifica se ci sono Brawler e Sniper
    bool HasBrawler = false;
    bool HasSniper = false;

    for (AActor* Unit : AIUnits)
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

    // Selezione casuale di una unità dall'array
    RandomIndex = FMath::RandRange(0, AIUnits.Num() - 1);
    AActor* RandomUnit = AIUnits[RandomIndex];

    // Controlla se l'unità è valida prima di procedere
    if (IsValid(RandomUnit))
    {
        // In base al tipo di unità, la salviamo e avviamo il movimento
        if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(RandomUnit))
        {
            SelectedBrawler = Brawler;
         //   GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("AI: Selezionato Brawler"));

            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveBrawler, 1.0f, false);
        }
        else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(RandomUnit))
        {
            SelectedSniper = Sniper;
          //  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("AI: Selezionato Sniper"));

            FTimerHandle TimerHandle;
            GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveSniper, 1.0f, false);
        }
        else
        {
           // UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer: L'unità selezionata non è né Brawler né Sniper."));
            GameMode->EndTurn();
        }
    }
    else
    {
       // UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer: L'unità selezionata non è più valida."));
        GameMode->EndTurn();
    }

    // Se non ci sono Brawler o Sniper, imposta i flag a true
    if (!HasBrawler)
    {
        bBrawlerMoved = true;
        bBrawlerAttackedThisTurn = true;

    }
    if (!HasSniper)
    {
        bSniperMoved = true;
        bSniperAttackedThisTurn = true;
    }
}


void AComputerPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void AComputerPlayer::PlaceUnit()
{
   // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
    GameIstance->SetTurnMessage(TEXT("AI (Random) Turn"));
    FTimerHandle TimerHandle;
    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
        {
            AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
            if (!GameMode)
            {
                UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameMode!"));
                return;
            }
            AGameField* GameField = GameMode->GameField;
            if (!GameField)
            {
                UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameField from GameMode!"));
                return;
            }
            int32 FieldSize = GameField->Size; // Use the Size from GameField
            int32 MaxAttempts = FieldSize * FieldSize; // Calculate max attempts
            for (int32 i = 0; i < MaxAttempts; ++i)
            {
                // 1. Choose a random tile using GetTile
                int32 RandomX = FMath::RandRange(0, FieldSize - 1); // Use FieldSize
                int32 RandomY = FMath::RandRange(0, FieldSize - 1); // Use FieldSize
                ATile* RandomTile = GameField->GetTile(RandomX, RandomY);
                // --- Debug Message for RandomX and RandomY ---
                if (GEngine)
                {
                    //FString DebugMessage = FString::Printf(TEXT("RandomX = %d, RandomY = %d"), RandomX, RandomY);
                    //GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMessage, true);
                }
                // --- End Debug Message ---
                // 2. Check if the tile is valid and empty
                if (RandomTile && RandomTile->GetTileStatus() == ETileStatus::EMPTY)
                {
                    // 3. Get the tile's position
                    FVector TileLocation = RandomTile->GetActorLocation();

                    /*
                    // --- Debug Drawing ---
                    float LineLength = GameField->GetTileSize() / 2.0f;
                    FVector LineStart1 = TileLocation + FVector(-LineLength, -LineLength, 50.0f);
                    FVector LineEnd1 = TileLocation + FVector(LineLength, LineLength, 50.0f);
                    FVector LineStart2 = TileLocation + FVector(-LineLength, LineLength, 50.0f);
                    FVector LineEnd2 = TileLocation + FVector(LineLength, -LineLength, 50.0f);
                    DrawDebugLine(GetWorld(), LineStart1, LineEnd1, FColor::Red, false, 5.0f, 0, 2.0f);
                    DrawDebugLine(GetWorld(), LineStart2, LineEnd2, FColor::Red, false, 5.0f, 0, 2.0f);
                    // --- End Debug Drawing ---
                    */
                    // 4. Call SetUnitPlacement
                    GameMode->SetUnitPlacement(1, TileLocation);
                    return;
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("ComputerPlayer: Failed to find an empty tile after %d attempts."), MaxAttempts);
            // Handle the case where no empty tile was found (e.g., log an error, delay and retry, etc.)
    }, 1.0f, false);
}





void AComputerPlayer::BeginPlay()
{
    Super::BeginPlay();

    
}






void AComputerPlayer::MoveBrawler()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (!SelectedBrawler)
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer::MoveBrawler - SelectedBrawler è nullptr"));
        return;
    }



    SelectedBrawler->TilesCanReach = SelectedBrawler->GetReachableTiles(SelectedBrawler->GetMovementRange());
    GameMode->GameField->HighlightReachableTiles(SelectedBrawler->TilesCanReach);

    TArray<ATile*> ReachableTiles = SelectedBrawler->TilesCanReach;                         //SelectedBrawler->GetReachableTiles(SelectedBrawler->GetMovementRange());
   


    if (ReachableTiles.Num() > 0)
    {
        RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
        ATile* TargetTile = ReachableTiles[RandomTileIndex];
        if (TargetTile)
        {
            //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Brawler si muove verso %s"), *TargetTile->GetName()));
            SelectedBrawler->MoveUnit(TargetTile);
           
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer::MoveBrawler - Nessuna tile raggiungibile per il Brawler"));
    }

    AAW_Brawler* BrawlerMoving = SelectedBrawler;

    // Imposta il timer utilizzando il membro BrawlerTimerHandle
    GetWorldTimerManager().SetTimer(BrawlerTimerHandle, [this, BrawlerMoving, GameMode]()
        {
            if (!BrawlerMoving->bIsMoving)
            {
                bBrawlerMoved = true;
                // Cancella il timer una volta che il movimento è completato
                GetWorldTimerManager().ClearTimer(BrawlerTimerHandle);
                GameMode->GameField->ClearHighlightedTiles(SelectedBrawler->TilesCanReach);

                PerformAttack(BrawlerMoving);
                ContinueWithNextUnit();
            }
            else
            {
               // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Brawler si sta muovendo"));
            }
        }, 1.0f, true);
}


void AComputerPlayer::MoveSniper()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!SelectedSniper)
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer::MoveSniper - SelectedSniper è nullptr"));
        return;
    }



    SelectedSniper->TilesCanReach = SelectedSniper->GetReachableTiles(SelectedSniper->GetMovementRange());
    GameMode->GameField->HighlightReachableTiles(SelectedSniper->TilesCanReach);

    TArray<ATile*> ReachableTiles = SelectedSniper->TilesCanReach;
    
 
    if (ReachableTiles.Num() > 0)
    {
        RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
        ATile* TargetTile = ReachableTiles[RandomTileIndex];
        if (TargetTile)
        {
          //  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Sniper si muove verso %s"), *TargetTile->GetName()));
            SelectedSniper->MoveUnit(TargetTile);
            
        }
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer::MoveSniper - Nessuna tile raggiungibile per lo Sniper"));
    }

    AAW_Sniper* SniperMoving = SelectedSniper;

    GetWorldTimerManager().SetTimer(SniperTimerHandle, [this, SniperMoving, GameMode]()
        {
            if (!SniperMoving->bIsMoving)
            {
                bSniperMoved = true;
                GetWorldTimerManager().ClearTimer(SniperTimerHandle);
                GameMode->GameField->ClearHighlightedTiles(SelectedSniper->TilesCanReach);

                PerformAttack(SniperMoving);
                ContinueWithNextUnit();
            }
            else
            {
              //  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sniper si sta muovendo"));
            }
        }, 1.0f, true);
}


void AComputerPlayer::ContinueWithNextUnit()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("AComputerPlayer::ContinueWithNextUnit - GameMode is null"));
        return;
    }

    // Se entrambe le unità hanno completato il movimento E l'attacco, termina il turno
    if (bBrawlerMoved && bSniperMoved && bBrawlerAttackedThisTurn && bSniperAttackedThisTurn)
    {
       // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI: Turno completato"));
        CurrentState = EComputerMoveState::Idle;
        GameMode->EndTurn();
        return;
    }

    // Se il Brawler ha mosso ma non ha attaccato, prova ad attaccare
    if (bBrawlerMoved && !bBrawlerAttackedThisTurn)
    {
        if (SelectedBrawler)
        {
            PerformAttack(SelectedBrawler);
        }
    }
    // Se lo Sniper ha mosso ma non ha attaccato, prova ad attaccare
    if (bSniperMoved && !bSniperAttackedThisTurn)
    {
        if (SelectedSniper)
        {
            PerformAttack(SelectedSniper);
        }
    }

    // Se il Brawler non ha ancora mosso, muovilo
    if (!bBrawlerMoved)
    {
        for (AActor* Unit : AIUnits)
        {
            if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit))
            {
                SelectedBrawler = Brawler;
                FTimerHandle TimerHandle;
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveBrawler, 1.0f, false);
                return;
            }
        }
    }

    // Se lo Sniper non ha ancora mosso, muovilo
    if (!bSniperMoved)
    {
        for (AActor* Unit : AIUnits)
        {
            if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit))
            {
                SelectedSniper = Sniper;
                FTimerHandle TimerHandle;
                GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveSniper, 1.0f, false);
                return;
            }
        }
    }
}





void AComputerPlayer::PerformAttack(AActor* AttackingUnit)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("AComputerPlayer::PerformAttack - GameMode is null"));
        return;
    }

    TArray<ATile*> AttackableTiles;

    if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(AttackingUnit))
    {
        AttackableTiles = Brawler->GetAttackableTiles();
    }
    else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(AttackingUnit))
    {
        AttackableTiles = Sniper->GetAttackableTiles();
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer::PerformAttack - Unit is not Brawler or Sniper"));
        return;
    }

    // Trova i nemici nelle tile attaccabili
    AActor* TargetEnemy = nullptr;
    ATile* TargetTile = nullptr;

    for (ATile* AttackTile : AttackableTiles)
    {
        if (AttackTile->GetUnit() && AttackTile->GetUnit()->GetOwner() != this)
        {
            TargetEnemy = AttackTile->GetUnit();
            TargetTile = AttackTile;
            break; // Trova il primo nemico e interrompi la ricerca
        }
    }

    if (TargetEnemy)
    {
        // Chiama la funzione Shoot sull'unità attaccante
        if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(AttackingUnit))
        {
            Brawler->Shoot(TargetTile);
            bBrawlerAttackedThisTurn = true;
          //  GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Brawler Attacking"));
        }
        else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(AttackingUnit))
        {
            Sniper->Shoot(TargetTile);
            bSniperAttackedThisTurn = true;
           // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sniper Attacking"));
        }
    }
    else
    {
        // Se non ci sono nemici, setta la flag come se avesse attaccato
        if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(AttackingUnit))
        {
            bBrawlerAttackedThisTurn = true;
        }
        else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(AttackingUnit))
        {
            bSniperAttackedThisTurn = true;
        }
    }
}