#include "Coin.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"

ACoin::ACoin()
{
    PrimaryActorTick.bCanEverTick = true;

    CoinMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoinMesh"));
    RootComponent = CoinMesh;
    CoinMesh->SetSimulatePhysics(true);
    CoinMesh->SetNotifyRigidBodyCollision(true); // Abilita gli eventi di collisione
}

void ACoin::BeginPlay()
{
    Super::BeginPlay();
    CoinMesh->OnComponentHit.AddDynamic(this, &ACoin::OnHit);
}

void ACoin::Flip()
{
    bHasLanded = false;

    // Reset physics state
    CoinMesh->SetPhysicsLinearVelocity(FVector::ZeroVector);
    //CoinMesh->SetPhysicsAngularVelocity(FVector::ZeroVector);
    CoinMesh->SetWorldLocation(FVector(0, 0, 200)); // Spawn above ground
    CoinMesh->SetWorldRotation(FRotator(0, 0, 0)); // Initial orientation

    // Apply upward impulse (simulates "flipping" force)
    FVector Impulse = FVector(0, 0, FMath::RandRange(300.f, 500.f)) * CoinMesh->GetMass();
    CoinMesh->AddImpulse(Impulse);

    // Apply random torque (makes it spin realistically)
    FVector Torque = FVector(
        FMath::RandRange(-2000.f, 2000.f),  // X-axis spin (optional)
        FMath::RandRange(-2000.f, 2000.f),  // Y-axis spin (optional)
        FMath::RandRange(1000.f, 3000.f)    // Z-axis spin (main flip rotation)
    );
    CoinMesh->AddTorqueInRadians(Torque * CoinMesh->GetMass());

    // Optional: Add slight horizontal movement for realism
    FVector LateralImpulse = FVector(
        FMath::RandRange(-50.f, 50.f),
        FMath::RandRange(-50.f, 50.f),
        0
    );
    CoinMesh->AddImpulse(LateralImpulse * CoinMesh->GetMass());
}




void ACoin::OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
    if (bHasLanded) return;

    // 1. Ottieni la velocità angolare attuale (metodo corretto)
    FVector AngularVelocity = CoinMesh->GetPhysicsAngularVelocityInRadians();

    // 2. Calcola l'energia cinetica residua
    float KineticEnergy = AngularVelocity.SizeSquared() * CoinMesh->GetMass();
    const float EnergyThreshold = 5.0f; // Soglia personalizzabile

    // 3. Se la moneta sta ancora ruotando significativamente, ignora
    if (KineticEnergy > EnergyThreshold)
    {
        return;
    }

    // 4. Determina il risultato (testa/croce)
    float UpDot = FVector::DotProduct(CoinMesh->GetUpVector(), FVector::UpVector);
    int32 Result = (UpDot > 0) ? 0 : 1;

    // 5. Gestisci l'atterraggio
    bHasLanded = true;
    OnCoinLanded.Broadcast(Result);

    // 6. Opzionale: Disabilita fisica e aggiungi effetti
    CoinMesh->SetSimulatePhysics(false);
    SpawnLandingEffects();
}

void ACoin::SpawnLandingEffects()
{
    // Particelle
    if (LandingParticles)
    {
        UGameplayStatics::SpawnEmitterAtLocation(
            GetWorld(),
            LandingParticles,
            GetActorLocation(),
            FRotator::ZeroRotator,
            true
        );
    }

    // Suono
    if (LandingSound)
    {
        UGameplayStatics::PlaySoundAtLocation(
            GetWorld(),
            LandingSound,
            GetActorLocation()
        );
    }
}