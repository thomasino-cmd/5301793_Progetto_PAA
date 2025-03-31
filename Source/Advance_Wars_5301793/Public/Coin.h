#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Coin.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API ACoin : public AActor
{
    GENERATED_BODY()

public:
    ACoin();

    // Delegato per risultato (0=Testa, 1=Croce)
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCoinLanded, int32, Result);
    UPROPERTY(BlueprintAssignable)
    FOnCoinLanded OnLanded;  

    // Funzioni essenziali
    UFUNCTION(BlueprintCallable)
    void LaunchCoin();

protected:
    virtual void BeginPlay() override;

    UFUNCTION()
    void OnHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
        UPrimitiveComponent* OtherComp, FVector NormalImpulse,
        const FHitResult& Hit);

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* CoinMesh;

private:
    bool bHasLanded = false;

    bool IsCoinStable() const;

    float ImpulseStrength;
    float TorqueStrength;

    UPROPERTY(EditAnywhere, Category = "Coin Physics")
    float RotationMultiplier ;
};