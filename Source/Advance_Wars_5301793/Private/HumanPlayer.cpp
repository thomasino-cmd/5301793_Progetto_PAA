#include "HumanPlayer.h"
#include "Tile.h"
#include "Obstacle.h"
#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

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


    if(GameMode->bIsPlacementPhaseOver == false)
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
    else //if(GameMode->bIsPlacementPhaseOver == true)
    {

        GEngine->AddOnScreenDebugMessage(-1, 9.f, FColor::Blue, TEXT("STARTING GAME PHASE"));
        GameIstance->SetTurnMessage(TEXT("STARTING GAME PHASE"));


        //  COMMENTO SENNO HA BISOGNO DI UN SECONDO CLICK PERCHE' IL PRIMO LO SPRECA PER ACCORGERSI CHE LA FASE DI POSIZIONAMENTO è FINITA 
       // FHitResult Hit;
       // GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, false, Hit);

        if (Hit.bBlockingHit)
        {
            if (ATile* ClickedTile = Cast<ATile>(Hit.GetActor()))
            {
                HandleTileClick(ClickedTile);
            }
            else if (AHumanPlayer* ClickedUnit = Cast<AHumanPlayer>(Hit.GetActor()))
            {
                HandleFriendlyUnitClick(ClickedUnit);
            }
            else if (AComputerPlayer* ClickedEnemyUnit = Cast<AComputerPlayer>(Hit.GetActor()))
            {
                HandleEnemyUnitClick(ClickedEnemyUnit);
            }
        }
    }
}




void AHumanPlayer::HandleTileClick(ATile* ClickedTile)
{
    AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
    if (bIsMyTurn)
    {
        // If human clicks on a Tile that belongs to him
        // => Set Tile selected (active)
        if (ClickedTile->GetTileOwner() == PlayerId)
        {
            const FVector2D Position = ClickedTile->GetGridPosition();
            GameMode->SetSelectedTile(Position);
        }

        // If the Tile clicked is legal
        // => Execute the move
       /* if (ClickedTile->IsLegalTile())
        {
            ExecuteTheMoveForHumanPlayer(ClickedTile);
        }*/
    }
}

void AHumanPlayer::HandleFriendlyUnitClick(AHumanPlayer* ClickedUnit)
{
    if (bIsMyTurn)
    {
        SelectUnit(ClickedUnit);
    }
}

void AHumanPlayer::HandleEnemyUnitClick(AComputerPlayer* ClickedEnemyUnit)
{
    if (bIsMyTurn && SelectedUnit)
    {
        AttackUnit(ClickedEnemyUnit);
    }
    else
    {
        // Optionally, handle clicking on an enemy unit when not ready to attack (e.g., show unit info)
        GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("Enemy Clicked: %s"), *ClickedEnemyUnit->GetName()));
    }
}

void AHumanPlayer::SelectUnit(AActor* Unit)
{
    SelectedUnit = Unit;
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("Unit Selected: %s"), *Unit->GetName()));
    // Add logic to highlight the unit or show movement range
}

void AHumanPlayer::MoveUnit(ATile* TargetTile)
{
    // Implement your unit movement logic here
    // This might involve checking for valid movement paths,
    // updating unit position, and potentially playing animations.
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Moving Unit to Tile"));
}

void AHumanPlayer::AttackUnit(AComputerPlayer* TargetUnit)
{
    // Implement your attack logic here
    // This could involve calculating damage, applying effects,
    // and handling unit destruction if applicable.
    GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Attacking Unit"));
}