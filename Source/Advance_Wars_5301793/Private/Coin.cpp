#include "Coin.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ACoin::ACoin()
{
    PrimaryActorTick.bCanEverTick = true;

    CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
    RootComponent = CoinMesh;
    CoinMesh->SetSimulatePhysics(false); // Fisica disattivata inizialmente
    CoinMesh->SetNotifyRigidBodyCollision(true);

    ImpulseStrength = 150.0f;
    TorqueStrength = 15000.0f;
    RotationMultiplier = FMath::RandRange(20.0f, 50.0f);
}

void ACoin::BeginPlay()
{
    Super::BeginPlay();
    CoinMesh->OnComponentHit.AddDynamic(this, &ACoin::OnHit);
}



void ACoin::LaunchCoin()
{
    if (!CoinMesh) return;

    // 1. Resetta posizione e orientamento
    FVector NewLocation = GetActorLocation();
    NewLocation.Z = 100.f;
    SetActorLocation(NewLocation);
    SetActorRotation(FRotator(0, 0, 0)); // Resetta la rotazione

    // 2. Attiva fisica
    CoinMesh->SetSimulatePhysics(true);
    bHasLanded = false;
    CoinMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    CoinMesh->SetPhysicsAngularVelocityInRadians(FVector::ZeroVector);

    // 3. Applica impulso verticale
    FVector Impulse = FVector(0, 0, ImpulseStrength * CoinMesh->GetMass());

    FVector Torque = FVector(
        TorqueStrength * CoinMesh->GetMass() * RotationMultiplier, // Asse X principale
        FMath::RandRange(-TorqueStrength * 0.2f, TorqueStrength * 0.2f) * CoinMesh->GetMass(), // Piccola variazione Y
        FMath::RandRange(-TorqueStrength * 0.1f, TorqueStrength * 0.1f) * CoinMesh->GetMass() // Piccola variazione Z
    );

    CoinMesh->AddImpulse(Impulse, NAME_None, true);
    CoinMesh->AddTorqueInRadians(Torque);

    // Debug
    //UE_LOG(LogTemp, Warning, TEXT("Applied Torque - X: %f, Y: %f, Z: %f"), Torque.X, Torque.Y, Torque.Z);
}



bool ACoin::IsCoinStable() const
{
    if (!CoinMesh) return false;

    // Get velocity in cm/s (Unreal units)
    const FVector LinearVelocity = CoinMesh->GetPhysicsLinearVelocity();
    // Get angular velocity in radians/s
    const FVector AngularVelocity = CoinMesh->GetPhysicsAngularVelocityInRadians();

    // Threshold values (adjust based on your needs)
    const float MaxLinearSpeed = 5.0f;  // 5 cm/s
    const float MaxAngularSpeed = 0.5f; // ~30 degrees/s

    return LinearVelocity.Size() < MaxLinearSpeed &&
        AngularVelocity.Size() < MaxAngularSpeed;
}

void ACoin::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
    UPrimitiveComponent* OtherComp, FVector NormalImpulse,
    const FHitResult& Hit)
{
    if (bHasLanded) return;

    // Only register landing when physics says we're stable
    if (IsCoinStable())
    {
        bHasLanded = true;
        const float UpDot = FVector::DotProduct(CoinMesh->GetUpVector(), FVector::UpVector);
        OnLanded.Broadcast(UpDot > 0 ? 0 : 1); // 0=Heads, 1=Tails

        // Optional: Freeze the coin
        CoinMesh->SetSimulatePhysics(false);
    }
}