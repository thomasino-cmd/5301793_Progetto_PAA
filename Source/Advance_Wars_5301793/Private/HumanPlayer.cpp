#include "HumanPlayer.h"
#include "Tile.h"
#include "Components/InputComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AHumanPlayer::AHumanPlayer()
{
    // Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Set this pawn to be controlled by the lowest-numbered player
    AutoPossessPlayer = EAutoReceiveInput::Player0;

    // Create a spring arm component
    SpringArm = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
    SpringArm->SetupAttachment(RootComponent);
    SpringArm->TargetArmLength = 500.0f;
    SpringArm->bUsePawnControlRotation = true;

    // Create a camera component
    Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
    Camera->SetupAttachment(SpringArm, USpringArmComponent::SocketName);
    Camera->bUsePawnControlRotation = false;

    //get the game istance refernce
    // GameIstnce = Cast<UTT_GameIstance>(UGameplayStatics::GetGameIstance(GetWorld()));
    //RootComponent = Camera;

    // Initialize default values
    PlayerId = 0; // Human player ID
    //bIsMyTurn = false;
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
    if (bIsMyTurn)
    {
        // Structure containing information about one hit of a trace, such as point of impact and surface normal at that point
        FHitResult Hit = FHitResult(ForceInit);

        // GetHitResultUnderCursor function sends a ray from the mouse position and gives the corresponding hit results
        GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

        if (Hit.bBlockingHit)
        {
            if (ATile* CurrTile = Cast<ATile>(Hit.GetActor()))
            {
                // GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Clicked on tile %d, %d"), CurrTile->GridPosition.X, CurrTile->GridPosition.Y);

                // Perform actions based on the clicked tile
                // ...

                //dipende se  la tileche clicco è : libera , occupata da : amico  nemico ostacolo 


                bIsMyTurn = false; 
            }
        }
    }
}