// Fill out your copyright notice in the Description page of Project Settings.
#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AW_Brawler.h"
#include "AW_Sniper.h"
    

AComputerPlayer::AComputerPlayer()
{
    PrimaryActorTick.bCanEverTick = true;
    GameIstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
    // default init values
    PlayerId = 1;
}

void AComputerPlayer::BeginPlay()
{
    Super::BeginPlay();

    // Ottieni un riferimento a GameField (assicurati che sia inizializzato correttamente)
    /*
    //  NON SO BENE A CHE SERVA  TOLTA PER DEBUGGARE SPAWN DEI GIOCATORI NEL GAMEMODE.CPP IL 10/03
     GameField = Cast<AGameField>(UGameplayStatics::GetActorOfClass(GetWorld(), AGameField::StaticClass()));
    if (!GameField)
    {
        UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Impossibile trovare GameField!"));
    }
    */
}


// Funzione Tick, chiamata ad ogni frame
void AComputerPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // Controlliamo lo stato corrente e attendiamo che il movimento dell'unità in questione termini
    if (CurrentState == EComputerMoveState::MovingBrawler)
    {
        if (SelectedBrawler && !SelectedBrawler->bIsMoving)
        {
            // Il Brawler ha finito di muovere, ora sposta lo Sniper
            CurrentState = EComputerMoveState::MovingSniper;
            MoveSniper();
        }
    }
    else if (CurrentState == EComputerMoveState::MovingSniper)
    {
        if (SelectedSniper && !SelectedSniper->bIsMoving)
        {
            // Lo Sniper ha finito di muovere: il turno è terminato
            CurrentState = EComputerMoveState::Finished;
            AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                GameMode->EndTurn();
            }
        }
    }
}

void AComputerPlayer::OnTurn()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (GameMode) // Always check if the pointer is valid!
    {
        if (GameMode->bIsPlacementPhaseOver == false)
        {
            PlaceUnit();
        }
        else if (GameMode->UnitsPlaced >= 4)
        {
            bBrawlerMoved = false;
            bSniperMoved = false;
            MakeMove();
        }
    }
    else
        {
            UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameMode!"));
        }
    
}


void AComputerPlayer::OnWin()
{
    // ... (Logica per la vittoria del computer) ...
}

void AComputerPlayer::OnLose()
{
    // ... (Logica per la sconfitta del computer) ...
}

void AComputerPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);
}



void AComputerPlayer::PlaceUnit()
{
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
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
                    FString DebugMessage = FString::Printf(TEXT("RandomX = %d, RandomY = %d"), RandomX, RandomY);
                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMessage, true);
                }
                // --- End Debug Message ---
                // 2. Check if the tile is valid and empty
                if (RandomTile && RandomTile->GetTileStatus() == ETileStatus::EMPTY)
                {
                    // 3. Get the tile's position
                    FVector TileLocation = RandomTile->GetActorLocation();
                    // --- Debug Drawing ---
                    float LineLength = GameField->GetTileSize() / 2.0f;
                    FVector LineStart1 = TileLocation + FVector(-LineLength, -LineLength, 50.0f);
                    FVector LineEnd1 = TileLocation + FVector(LineLength, LineLength, 50.0f);
                    FVector LineStart2 = TileLocation + FVector(-LineLength, LineLength, 50.0f);
                    FVector LineEnd2 = TileLocation + FVector(LineLength, -LineLength, 50.0f);
                    DrawDebugLine(GetWorld(), LineStart1, LineEnd1, FColor::Red, false, 5.0f, 0, 2.0f);
                    DrawDebugLine(GetWorld(), LineStart2, LineEnd2, FColor::Red, false, 5.0f, 0, 2.0f);
                    // --- End Debug Drawing ---
                    // 4. Call SetUnitPlacement
                    GameMode->SetUnitPlacement(1, TileLocation);
                    //GameMode->EndTurn();
                    // Exit the function after placing the unit
                    return;
                }
            }
            UE_LOG(LogTemp, Warning, TEXT("ComputerPlayer: Failed to find an empty tile after %d attempts."), MaxAttempts);
            // Handle the case where no empty tile was found (e.g., log an error, delay and retry, etc.)
        }, 1, false);
}






// Funzione MakeMove, chiamata all'inizio del turno dell'AI
void AComputerPlayer::MakeMove()
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode or GameField is null in AComputerPlayer::MakeMove"));
        return;
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
    GameIstance->SetTurnMessage(TEXT("AI Turn"));

    AIUnits = GameMode->GetCurrentPlayerUnits(PlayerId);
    if (AIUnits.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AI has no units to move. Ending turn."));
        GameMode->EndTurn();
        return;
    }

    // Scegliamo in modo casuale un indice tra 0 e AIUnits.Num()-1.
    int32 RandomIndex = FMath::RandRange(0, AIUnits.Num() - 1);
    AActor* RandomUnit = AIUnits[RandomIndex];

    // Se l'unità casuale è un Brawler, muoviamo prima il Brawler, altrimenti lo Sniper.
    if (Cast<AAW_Brawler>(RandomUnit))
    {
        bBrawlerFirst = true;
    }
    else if (Cast<AAW_Sniper>(RandomUnit))
    {
        bBrawlerFirst = false;
    }
    else
    {
        // Se per qualche ragione l'unità non è né Brawler né Sniper, default a Brawler.
        bBrawlerFirst = true;
    }

    // Avvio del movimento in base alla scelta casuale
    if (bBrawlerFirst)
    {
        CurrentState = EComputerMoveState::MovingBrawler;
        MoveBrawler();
    }
    else
    {
        CurrentState = EComputerMoveState::MovingSniper;
        MoveSniper();
    }
}

// Funzione per far muovere il Brawler
void AComputerPlayer::MoveBrawler()
{
    SelectedBrawler = nullptr;
    // Cerchiamo tra le unità disponibili un Brawler che possa muovere
    for (AActor* Unit : AIUnits)
    {
        AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit);
        if (Brawler)
        {
            TArray<ATile*> ReachableTiles = Brawler->GetReachableTiles(Brawler->GetMovementRange());
            if (ReachableTiles.Num() > 0)
            {
                int32 RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
                ATile* TargetTile = ReachableTiles[RandomTileIndex];
                Brawler->MoveUnit(TargetTile);
                SelectedBrawler = Brawler;
                UE_LOG(LogTemp, Log, TEXT("AI moving Brawler: %s"), *Brawler->GetName());
                return;
            }
        }
    }

    // Se nessun Brawler può muovere, passa allo Sniper
    CurrentState = EComputerMoveState::MovingSniper;
    MoveSniper();
}

// Funzione per far muovere lo Sniper
void AComputerPlayer::MoveSniper()
{
    SelectedSniper = nullptr;
    // Cerchiamo tra le unità disponibili uno Sniper che possa muovere
    for (AActor* Unit : AIUnits)
    {
        AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit);
        if (Sniper)
        {
            TArray<ATile*> ReachableTiles = Sniper->GetReachableTiles(Sniper->GetMovementRange());
            if (ReachableTiles.Num() > 0)
            {
                int32 RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
                ATile* TargetTile = ReachableTiles[RandomTileIndex];
                Sniper->MoveUnit(TargetTile);
                SelectedSniper = Sniper;
                UE_LOG(LogTemp, Log, TEXT("AI moving Sniper: %s"), *Sniper->GetName());
                return;
            }
        }
    }

    // Se nemmeno lo Sniper può muovere, termina il turno
    CurrentState = EComputerMoveState::Finished;
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (GameMode)
    {
        GameMode->EndTurn();
    }
}







//void AComputerPlayer::MoveAIUnits(TArray<AActor*> AIUnits, int32 UnitIndex, float Delay)
//{
//    if (UnitIndex >= AIUnits.Num())
//    {
//        // Tutte le unità si sono mosse
//        GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
//            AAWGameMode* GameMode = (AAWGameMode*)(GetWorld()->GetAuthGameMode());
//            if (GameMode)
//            {
//                GameMode->EndTurn(); // Segnala la fine del turno dell'AI
//            }
//            });
//        return;
//    }
//    AActor* Unit = AIUnits[UnitIndex];
//
//    AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit);
//    AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit);
//
//    if (Brawler && !bBrawlerMoved)
//    {
//        TArray<ATile*> ReachableTiles = Brawler->GetReachableTiles(Brawler->GetMovementRange());
//        if (ReachableTiles.Num() > 0)
//        {
//            int32 RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
//            ATile* TargetTile = ReachableTiles[RandomTileIndex];
//
//            Brawler->MoveUnit(TargetTile);
//            bBrawlerMoved = true;
//
//            // Attendi che il Brawler abbia finito di muoversi
//            GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, [this, AIUnits, UnitIndex, Delay]() {
//                if (!Cast<AAW_Brawler>(AIUnits[UnitIndex])->bIsMoving)
//                {
//                    // Movimento del Brawler completato, attendi un secondo di "ragionamento"
//                    GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, [this, AIUnits, UnitIndex, Delay]() {
//                        // Muovi la prossima unità
//                        MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
//                        }, 1.0f, false);
//                }
//                else
//                {
//                    // Movimento ancora in corso, ricontrolla
//                    GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, FTimerDelegate::CreateLambda([this, AIUnits, UnitIndex, Delay]() {
//                        MoveAIUnits(AIUnits, UnitIndex, Delay);
//                        }), 0.1f, false);
//                }
//                }, 0.1f, false);
//        }
//        else
//        {
//            // Nessuna tile raggiungibile, passa all'unità successiva
//            MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
//        }
//    }
//    else if (Sniper && !bSniperMoved)
//    {
//        TArray<ATile*> ReachableTiles = Sniper->GetReachableTiles(Sniper->GetMovementRange());
//        if (ReachableTiles.Num() > 0)
//        {
//            int32 RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
//            ATile* TargetTile = ReachableTiles[RandomTileIndex];
//
//            Sniper->MoveUnit(TargetTile);
//            bSniperMoved = true;
//
//            // Attendi che lo Sniper abbia finito di muoversi
//            GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, [this, AIUnits, UnitIndex, Delay]() {
//                if (!Cast<AAW_Sniper>(AIUnits[UnitIndex])->bIsMoving)
//                {
//                    // Movimento dello Sniper completato, attendi un secondo di "ragionamento"
//                    GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, [this, AIUnits, UnitIndex, Delay]() {
//                        // Muovi la prossima unità
//                        MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
//                        }, 1.0f, false);
//                }
//                else
//                {
//                    // Movimento ancora in corso, ricontrolla
//                    GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, FTimerDelegate::CreateLambda([this, AIUnits, UnitIndex, Delay]() {
//                        MoveAIUnits(AIUnits, UnitIndex, Delay);
//                        }), 0.1f, false);
//                }
//                }, 0.1f, false);
//        }
//        else
//        {
//            // Nessuna tile raggiungibile, passa all'unità successiva
//            MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
//        }
//    }
//    else
//    {
//        // Unità già mossa o non Brawler/Sniper, passa all'unità successiva
//        MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
//    }
//}