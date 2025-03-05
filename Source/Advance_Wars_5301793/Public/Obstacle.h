#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Obstacle.generated.h"


// Enum to define the different types of obstacles
UENUM(BlueprintType)
enum class EObstacleType : uint8
{
    Mountain UMETA(DisplayName = "Mountain"),
    Tree UMETA(DisplayName = "Tree"),
    // ... other members ...
};


UCLASS()
class ADVANCE_WARS_5301793_API AObstacle : public AActor
{
    GENERATED_BODY()

public:
    // Sets default values for this actor's properties
    AObstacle();
    
    // Obstacle Type (Mountain, Tree, etc.)
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Obstacle")
    uint8 ObstacleType;

    // Function to set the obstacle type
    UFUNCTION(BlueprintCallable, Category = "Obstacle")
    void SetObstacleType(EObstacleType NewType);
    
protected:
    // Called when the game starts or when spawned
    virtual void BeginPlay() override;

    UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Obstacle")
    AActor* SelectedObstacle;

public:
    // Called every frame
    virtual void Tick(float DeltaTime) override;

    // Static Mesh Component for the obstacle's visual representation
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
    UStaticMeshComponent* ObstacleMesh;
};