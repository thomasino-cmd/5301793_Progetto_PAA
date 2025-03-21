// Fill out your copyright notice in the Description page of Project Settings.
#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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

void AComputerPlayer::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
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









void AComputerPlayer::MakeMove()
{
    AAWGameMode* GameMode = (AAWGameMode*)(GetWorld()->GetAuthGameMode());
    if (!GameMode || !GameMode->GameField)
    {
        UE_LOG(LogTemp, Error, TEXT("GameMode or GameField is null in AComputerPlayer::MakeMove"));
        return;
    }

    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Delayed) Turn"));
    GameIstance->SetTurnMessage(TEXT("AI Turn"));

    TArray<AActor*> AIUnits = GameMode->GetCurrentPlayerUnits(PlayerId);

    if (AIUnits.Num() == 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("AI has no units to move. Ending turn."));
        GameMode->EndTurn();
        return;
    }

    bBrawlerMoved = false;
    bSniperMoved = false;
    float AIDelay = 2.0f; // Delay in seconds between unit movements

    MoveAIUnits(AIUnits, 0, AIDelay); // Start the movement sequence
}







void AComputerPlayer::MoveAIUnits(TArray<AActor*> AIUnits, int32 UnitIndex, float Delay)
{
    if (UnitIndex >= AIUnits.Num())
    {
        // All units have been moved
        GetWorld()->GetTimerManager().SetTimerForNextTick([this]() {
            AAWGameMode* GameMode = (AAWGameMode*)(GetWorld()->GetAuthGameMode());
            if (GameMode)
            {
                GameMode->EndTurn(); // Signal end of AI movement
            }
            });
        return;
    }

    AActor* Unit = AIUnits[UnitIndex];

    AAW_Brawler* Brawler = Cast<AAW_Brawler>(Unit);
    AAW_Sniper* Sniper = Cast<AAW_Sniper>(Unit);

    if (Brawler && !bBrawlerMoved)
    {
        TArray<ATile*> ReachableTiles = Brawler->GetReachableTiles(Brawler->GetMovementRange());
        if (ReachableTiles.Num() > 0)
        {
            int32 RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
            ATile* TargetTile = ReachableTiles[RandomTileIndex];
            Brawler->MoveUnit(TargetTile);
            bBrawlerMoved = true;

            // Wait for Brawler to finish moving using timer
            GetWorld()->GetTimerManager().SetTimer(
                MovementTimerHandle,
                [this, AIUnits, UnitIndex, Delay, Brawler]() {
                    if (!Brawler->bIsMoving)
                    {
                        // Movement finished, move to the next unit
                        MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
                    }
                    else
                    {
                        //// Movement still in progress, recheck
                        //GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, FTimerDelegate::CreateLambda([this, AIUnits, UnitIndex, Delay, Brawler]() {
                        //    MoveAIUnits(AIUnits, UnitIndex, Delay);
                        //    }), 0.1f, false);
                        MoveAIUnits(AIUnits, UnitIndex, Delay);

                    }
                },
                1.0f,
                false
            );
        }
        else
        {
            MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
        }

    }
    else if (Sniper && !bSniperMoved)
    {
        TArray<ATile*> ReachableTiles = Sniper->GetReachableTiles(Sniper->GetMovementRange());
        if (ReachableTiles.Num() > 0)
        {
            int32 RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
            ATile* TargetTile = ReachableTiles[RandomTileIndex];
            Sniper->MoveUnit(TargetTile);
            bSniperMoved = true;

            // Wait for Sniper to finish moving using timer
            GetWorld()->GetTimerManager().SetTimer(
                MovementTimerHandle,
                [this, AIUnits, UnitIndex, Delay, Sniper]() {
                    if (!Sniper->bIsMoving)
                    {
                        // Movement finished, move to the next unit
                        MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
                    }
                    else
                    {
                        /*GetWorld()->GetTimerManager().SetTimer(MovementTimerHandle, FTimerDelegate::CreateLambda([this, AIUnits, UnitIndex, Delay, Sniper]() {
                            MoveAIUnits(AIUnits, UnitIndex, Delay);
                            }), 0.1f, false);*/

                        MoveAIUnits(AIUnits, UnitIndex, Delay);
                    }
                },
                1.0f,
                false
            );
        }
        else
        {
            MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
        }
    }
    else
    {
        MoveAIUnits(AIUnits, UnitIndex + 1, Delay);
    }
}