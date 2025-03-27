#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"


class UParticleSystem;
class USoundBase;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinLanded, int32, Result); // 0 = Testa (Player), 1 = Croce (AI)

UCLASS()
class ADVANCE_WARS_5301793_API ACoin : public AActor
{
    GENERATED_BODY()

public:
    ACoin();

    UPROPERTY(BlueprintAssignable, Category = "Coin")
    FOnCoinLanded OnCoinLanded;

    UFUNCTION(BlueprintCallable, Category = "Coin")
    void Flip(); // Avvia il lancio

    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    UParticleSystem* LandingParticles;

    /** Suono da riprodurre quando la moneta atterra */
    UPROPERTY(EditDefaultsOnly, Category = "Effects")
    USoundBase* LandingSound;

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

    /** Funzione per spawnare effetti visivi/audio */
    UFUNCTION(BlueprintCallable, Category = "Effects")
    void SpawnLandingEffects();



    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* CoinMesh;

    bool bHasLanded = false;
};