#include "HumanPlayer.h"
#include "Tile.h"
#include "Obstacle.h"
#include "ComputerPlayer.h"
#include "AWGameMode.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#include "Kismet/GameplayStatics.h"


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

    /*
    // Enhanced Input
    if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetWorld()))
    {
        Subsystem->AddMappingContext(InputMappingContext, 0);
    }

    if (APlayerController* PlayerController = Cast<APlayerController>(GetController()))
    {
        if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
        {
            EnhancedInputComponent->BindAction(ClickAction, ETriggerEvent::Triggered, this, &AHumanPlayer::OnClick);
        }
    }
    */
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




void AHumanPlayer::OnClick()
{
    /*
    if (bIsMyTurn)
    {
        // 1. Get mouse position and convert to world coordinates.
        FVector2D MousePosition;
        APlayerController* PC = GetWorld()->GetFirstPlayerController();
        if (PC)
        {
            PC->GetMousePosition(MousePosition.X, MousePosition.Y);
        }
        else
        {
            return; // No PlayerController found
        }

        // 2. Perform line trace to detect clicked tile.
        FHitResult HitResult;
        PC->GetHitResultUnderCursor(ECC_Visibility, false, HitResult);

        if (HitResult.bBlockingHit)
        {
            ATile* ClickedTile = Cast<ATile>(HitResult.GetActor());
            if (ClickedTile)
            {
                AActor* ClickedActor = ClickedTile->GetUnit();

                if (ClickedActor)
                {
                    if (AHumanPlayer* ClickedUnit = Cast<AHumanPlayer>(ClickedActor))
                    {
                        // Handle human player unit (should not happen, clicking on yourself)
                    }
                    else if (AComputerPlayer* ClickedComputerUnit = Cast<AComputerPlayer>(ClickedActor))
                    {
                        // Handle AI player unit (e.g., attack)
                        if (SelectedUnit)
                        {
                            AttackUnit(ClickedComputerUnit);
                        }
                    }
                    else if (AObstacle* ClickedObstacle = Cast<AObstacle>(ClickedActor))
                    {
                        // Handle obstacle (e.g., show info)
                    }
                    // ... handle other types as needed
                }
                else
                {
                    // Handle empty tile (e.g., move selected unit)
                    if (SelectedUnit)
                    {
                        MoveUnit(ClickedTile);
                    }
                }
            }
        }

        bIsMyTurn = false; // End turn after processing the click
    }
    */

    //Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
    FHitResult Hit = FHitResult(ForceInit);
    // GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
    GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Pawn, true, Hit);
    if (Hit.bBlockingHit && bIsMyTurn)
    {
        if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
        {
            if (CurrTile->GetTileStatus() == ETileStatus::EMPTY)
            {
                GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("clicked"));
                CurrTile->SetTileStatus(PlayerId, ETileStatus::OCCUPIED);
                FVector SpawnPosition = CurrTile->GetActorLocation();
                AAWGameMode* GameMode = Cast<AAWGameMode>(GetWorld()->GetAuthGameMode());
                GameMode->SetUnitPlacement(PlayerId, SpawnPosition);
                bIsMyTurn = false;
            }
        }
    }
}