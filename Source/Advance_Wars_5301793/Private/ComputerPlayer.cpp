//// Fill out your copyright notice in the Description page of Project Settings.
//#include "ComputerPlayer.h"
//#include "AWGameMode.h"
//#include "Kismet/GameplayStatics.h"
//#include "Components/InputComponent.h"
//#include "EnhancedInputComponent.h"
//#include "EnhancedInputSubsystems.h"
//#include "AW_Brawler.h"
//#include "AW_Sniper.h"
//    
//
//AComputerPlayer::AComputerPlayer()
//{
//    PrimaryActorTick.bCanEverTick = true;
//    GameIstance = Cast<UAWGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
//    // default init values
//    PlayerId = 1;
//
//
//    SelectedBrawler = nullptr; 
//    SelectedSniper = nullptr;
//
//
//    RandomIndex = 0;
//    RandomTileIndex = 0;
//}
//
//void AComputerPlayer::BeginPlay()
//{
//    Super::BeginPlay();
//
//    
//}
//
//
//// Funzione Tick, chiamata ad ogni frame
//void AComputerPlayer::Tick(float DeltaTime)
//{
//    Super::Tick(DeltaTime);
//
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//    if (!GameMode)
//    {
//        return;
//    }
//
//
//
//
//    switch (CurrentState)
//    {
//    case EComputerMoveState::MovingBrawler:
//        if (SelectedBrawler && !SelectedBrawler->bIsMoving && bBrawlerMoved) // Check if Brawler has finished moving
//        {
//            //bBrawlerMoved = true;
//
//            if (bSniperMoved==false)
//            {
//                CurrentState = EComputerMoveState::MovingSniper;
//                SelectedSniper = Cast<AAW_Sniper>(AIUnits[1]); // Assuming Sniper is the second unit
//                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, TEXT("macchina va a muovere lo sniper"));
//                MoveSniper();
//            }
//            else
//            {
//                CurrentState = EComputerMoveState::Finished;
//                  
//            }
//        }
//        else
//        {
//           // MoveBrawler();
//        }
//        break;
//    case EComputerMoveState::MovingSniper:
//        if (SelectedSniper && !SelectedSniper->bIsMoving && bSniperMoved) // Check if Sniper has finished moving
//        {
//            //bSniperMoved = true;
//
//            if (bBrawlerMoved == true)   
//            {
//                CurrentState = EComputerMoveState::Finished;
//            }
//            else
//            {
//                CurrentState = EComputerMoveState::MovingBrawler;
//                SelectedBrawler = Cast<AAW_Brawler>(AIUnits[0]);
//                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("macchina va a muovere il brawler"));
//                MoveBrawler();
//            }
//   
//        }
//        else
//        {
//          //  MoveSniper();
//        }
//        break;
//
//
//
//    case EComputerMoveState::Finished:
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("macchina finisce turno"));
//        GameMode->EndTurn();
//        break;
//    case EComputerMoveState::Idle:
//        // Waiting for turn to start
//       // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("macchina idle"));
//        break;
//
//    case EComputerMoveState::Moving:
//        GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Black, TEXT("iniziando movimento"));
//        //MakeMove();
//        break;
//    }
//
//}
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//void AComputerPlayer::OnTurn()
//{
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//
//    CurrentState = EComputerMoveState::Idle;
//
//
//    if (GameMode) // Always check if the pointer is valid!
//    {
//        if (GameMode->bIsPlacementPhaseOver == false)
//        {
//            PlaceUnit();
//        }
//        else if (GameMode->UnitsPlaced >= 4)
//        {
//            bBrawlerMoved = false;
//            bSniperMoved = false;
//
//
//            //temporanemanete piazzo qui 
//
//            //RandomIndex = 0;
//            //RandomTileIndex = 3;
//
//            CurrentState = EComputerMoveState::Moving;
//
//            MakeMove();
//
//            //GameMode->EndTurn();
//        }
//    }
//    else
//        {
//            UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameMode!"));
//        }
//    
//}
//
//
//void AComputerPlayer::OnWin()
//{
//    // ... (Logica per la vittoria del computer) ...
//}
//
//void AComputerPlayer::OnLose()
//{
//    // ... (Logica per la sconfitta del computer) ...
//}
//
//void AComputerPlayer::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
//{
//    Super::SetupPlayerInputComponent(PlayerInputComponent);
//}
//
//
//
//void AComputerPlayer::PlaceUnit()
//{
//    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
//    GameIstance->SetTurnMessage(TEXT("AI (Random) Turn"));
//    FTimerHandle TimerHandle;
//    GetWorld()->GetTimerManager().SetTimer(TimerHandle, [&]()
//        {
//            AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//            if (!GameMode)
//            {
//                UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameMode!"));
//                return;
//            }
//            AGameField* GameField = GameMode->GameField;
//            if (!GameField)
//            {
//                UE_LOG(LogTemp, Error, TEXT("ComputerPlayer: Could not get GameField from GameMode!"));
//                return;
//            }
//            int32 FieldSize = GameField->Size; // Use the Size from GameField
//            int32 MaxAttempts = FieldSize * FieldSize; // Calculate max attempts
//            for (int32 i = 0; i < MaxAttempts; ++i)
//            {
//                // 1. Choose a random tile using GetTile
//                int32 RandomX = FMath::RandRange(0, FieldSize - 1); // Use FieldSize
//                int32 RandomY = FMath::RandRange(0, FieldSize - 1); // Use FieldSize
//                ATile* RandomTile = GameField->GetTile(RandomX, RandomY);
//                // --- Debug Message for RandomX and RandomY ---
//                if (GEngine)
//                {
//                    FString DebugMessage = FString::Printf(TEXT("RandomX = %d, RandomY = %d"), RandomX, RandomY);
//                    GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Yellow, DebugMessage, true);
//                }
//                // --- End Debug Message ---
//                // 2. Check if the tile is valid and empty
//                if (RandomTile && RandomTile->GetTileStatus() == ETileStatus::EMPTY)
//                {
//                    // 3. Get the tile's position
//                    FVector TileLocation = RandomTile->GetActorLocation();
//                    // --- Debug Drawing ---
//                    float LineLength = GameField->GetTileSize() / 2.0f;
//                    FVector LineStart1 = TileLocation + FVector(-LineLength, -LineLength, 50.0f);
//                    FVector LineEnd1 = TileLocation + FVector(LineLength, LineLength, 50.0f);
//                    FVector LineStart2 = TileLocation + FVector(-LineLength, LineLength, 50.0f);
//                    FVector LineEnd2 = TileLocation + FVector(LineLength, -LineLength, 50.0f);
//                    DrawDebugLine(GetWorld(), LineStart1, LineEnd1, FColor::Red, false, 5.0f, 0, 2.0f);
//                    DrawDebugLine(GetWorld(), LineStart2, LineEnd2, FColor::Red, false, 5.0f, 0, 2.0f);
//                    // --- End Debug Drawing ---
//                    // 4. Call SetUnitPlacement
//                    GameMode->SetUnitPlacement(1, TileLocation);
//                    //GameMode->EndTurn();
//                    // Exit the function after placing the unit
//                    return;
//                }
//            }
//            UE_LOG(LogTemp, Warning, TEXT("ComputerPlayer: Failed to find an empty tile after %d attempts."), MaxAttempts);
//            // Handle the case where no empty tile was found (e.g., log an error, delay and retry, etc.)
//        }, 1, false);
//}
//
//
//
//
//
//
//// Funzione MakeMove, chiamata all'inizio del turno dell'AI
//void AComputerPlayer::MakeMove()
//{
//    CurrentState = EComputerMoveState::Idle;
//    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
//    if (!GameMode || !GameMode->GameField)
//    {
//        UE_LOG(LogTemp, Error, TEXT("GameMode or GameField is null in AComputerPlayer::MakeMove"));
//        return;
//    }
//
//    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI (Random) Turn"));
//    GameIstance->SetTurnMessage(TEXT("AI Turn"));
//
//    AIUnits = GameMode->GetCurrentPlayerUnits(PlayerId);
//    if (AIUnits.Num() == 0)
//    {
//        UE_LOG(LogTemp, Warning, TEXT("AI has no units to move. Ending turn."));
//
//        //todo ending game se mai 
//        GameMode->EndTurn();
//        return;
//    }
//
//    // Scegliamo in modo casuale un indice tra 0 e AIUnits.Num()-1.
//    RandomIndex = FMath::RandRange(0, AIUnits.Num() - 1);
//
//
//
//    AActor* RandomUnit = AIUnits[RandomIndex];
//
//    // Se l'unità casuale è un Brawler, muoviamo prima il Brawler, altrimenti lo Sniper.
//    if (Cast<AAW_Brawler>(RandomUnit))
//    {
//        SelectedBrawler = Cast<AAW_Brawler>(RandomUnit);
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("AI Moves Brawler"));
//        
//        
//        CurrentState = EComputerMoveState::MovingBrawler;
//
//
//        this->MoveBrawler();
//
//       // 
//       // if (RandomIndex == 0) {
//       //     RandomIndex++;
//       // }
//       // else if (RandomIndex == 1) {
//       //     RandomIndex = 0;
//       // }
//
//
//       // RandomUnit = AIUnits[RandomIndex];
//
//       // SelectedSniper = Cast<AAW_Sniper>(RandomUnit);
//       // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("AI Moves sniper"));
//       // CurrentState = EComputerMoveState::MovingSniper;
//       // this->MoveSniper();
//
//       // CurrentState = EComputerMoveState::Finished;
//       // GameMode->EndTurn();
//        
//        return;
//
//    }
//    else if (Cast<AAW_Sniper>(RandomUnit))
//    {
//        SelectedSniper = Cast<AAW_Sniper>(RandomUnit);
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("AI Moves sniper"));
//       
//        
//        CurrentState = EComputerMoveState::MovingSniper;
//
//
//        this->MoveSniper();
//
//
//        //if (RandomIndex == 0) {
//        //    RandomIndex++;
//        //}
//        //else if (RandomIndex == 1) {
//        //    RandomIndex = 0;
//        //}
//
//        //RandomUnit = AIUnits[RandomIndex];
//
//        //SelectedBrawler = Cast<AAW_Brawler>(RandomUnit);
//        //GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("AI Moves Brawler"));
//        //CurrentState = EComputerMoveState::MovingBrawler;
//        //this->MoveBrawler();
//
//
//        //CurrentState = EComputerMoveState::Finished;
//        ////GameMode->EndTurn();
//
//        return;
//    }
//
//   
//}
//
//// Funzione per far muovere il Brawler
//void AComputerPlayer::MoveBrawler()
//{
//    TArray<ATile*> ReachableTiles = SelectedBrawler->GetReachableTiles(SelectedBrawler->GetMovementRange());
//    if (ReachableTiles.Num() > 0)
//    {
//        RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
//        ATile* TargetTile = ReachableTiles[RandomTileIndex];
//
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("moving to %s"), *TargetTile->GetName()));
//  
//        SelectedBrawler->MoveUnit(TargetTile);
//
//
//
//        if (SelectedBrawler->bIsMoving == false)
//        {
//            //CurrentState = EComputerMoveState::MovingBrawler;
//            bBrawlerMoved = true;
//            return;
//        }
//        
//
//
//        //UE_LOG(LogTemp, Log, TEXT("AI moving Brawler: %s"), *SelectedBrawler->GetName());
//        //return;
//    }
//
//}
//
//// Funzione per far muovere lo Sniper
//void AComputerPlayer::MoveSniper()
//{
//    TArray<ATile*> ReachableTiles = SelectedSniper->GetReachableTiles(SelectedSniper->GetMovementRange());
//    if (ReachableTiles.Num() > 0)
//    {
//        RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
//        ATile* TargetTile = ReachableTiles[RandomTileIndex];
//
//
//
//        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("moving to %s"), *TargetTile->GetName()));
//
//        SelectedSniper->MoveUnit(TargetTile);
//
//        if (SelectedSniper->bIsMoving == false)
//        {
//            //CurrentState = EComputerMoveState::MovingSniper;
//            bSniperMoved = true;
//            return;
//
//        }
//
//
//        //UE_LOG(LogTemp, Log, TEXT("AI moving sniper: %s"), *SelectedSniper->GetName());
//        //return;
//    }
//}



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
                GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("AI: turno di MUOVERE"));
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
    

    // Recuperiamo il GameMode e le unità AI tramite il metodo che usavi tu
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());

    if (!GameMode)
    {
        UE_LOG(LogTemp, Error, TEXT("AComputerPlayer::OnTurn - GameMode nullo"));
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

    // Selezione casuale di una unità dall'array
    RandomIndex = FMath::RandRange(0, AIUnits.Num() - 1);
    AActor* RandomUnit = AIUnits[RandomIndex];

    // In base al tipo di unità, la salviamo e avviamo il movimento
    if (AAW_Brawler* Brawler = Cast<AAW_Brawler>(RandomUnit))
    {
        SelectedBrawler = Brawler;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, TEXT("AI: Selezionato Brawler"));


      /*  SelectedBrawler->TilesCanReach = SelectedBrawler->GetReachableTiles(SelectedBrawler->GetMovementRange());
        GameMode->GameField->HighlightReachableTiles(SelectedBrawler->TilesCanReach);*/


        // Avvia il movimento del Brawler con un delay di 1 secondo
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveBrawler, 1.0f, false);
    }
    else if (AAW_Sniper* Sniper = Cast<AAW_Sniper>(RandomUnit))
    {
        SelectedSniper = Sniper;
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, TEXT("AI: Selezionato Sniper"));

        //SelectedSniper->TilesCanReach = SelectedSniper->GetReachableTiles(SelectedSniper->GetMovementRange());
        //GameMode->GameField->HighlightReachableTiles(SelectedSniper->TilesCanReach);

        // Avvia il movimento dello Sniper con un delay di 1 secondo
        FTimerHandle TimerHandle;
        GetWorldTimerManager().SetTimer(TimerHandle, this, &AComputerPlayer::MoveSniper, 1.0f, false);
    }
    else
    {
        UE_LOG(LogTemp, Warning, TEXT("AComputerPlayer: L'unità selezionata non è né Brawler né Sniper."));
        GameMode->EndTurn();
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
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Emerald, FString::Printf(TEXT("Brawler si muove verso %s"), *TargetTile->GetName()));
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
                ContinueWithNextUnit();
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Brawler si sta muovendo"));
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
    
 
    
    //GetReachableTiles(SelectedSniper->GetMovementRange());
    if (ReachableTiles.Num() > 0)
    {
        RandomTileIndex = FMath::RandRange(0, ReachableTiles.Num() - 1);
        ATile* TargetTile = ReachableTiles[RandomTileIndex];
        if (TargetTile)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("Sniper si muove verso %s"), *TargetTile->GetName()));
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
                ContinueWithNextUnit();
            }
            else
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Sniper si sta muovendo"));
            }
        }, 1.0f, true);
}


void AComputerPlayer::ContinueWithNextUnit()
{
    // Se entrambe le unità hanno completato il movimento, termina il turno
    if (bBrawlerMoved && bSniperMoved)
    {
        AAWGameMode* GameMode = Cast<AAWGameMode>(UGameplayStatics::GetGameMode(GetWorld()));
        if (GameMode)
        {
            GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("AI: Turno completato"));

            CurrentState = EComputerMoveState::Idle;
            bTurnEnded = true;
            GameMode->EndTurn();
        }
        return;
    }

    // Se invece una sola unità (es. quella selezionata casualmente) ha effettuato il movimento,
    // controlla se è presente l'altra unità. Se non era stata scelta inizialmente, la cerca fra quelle AI.
    if (!bBrawlerMoved )
    {
        // Cerca tra le unità un Brawler che non abbia ancora mosso (questo esempio presuppone che ne esista almeno uno)
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
    if (!bSniperMoved )                           //ERA : (!bSniperMoved && !SelectedSniper)
    {
        // Cerca tra le unità uno Sniper
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


