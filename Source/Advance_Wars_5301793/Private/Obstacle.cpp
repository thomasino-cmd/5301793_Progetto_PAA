#include "Obstacle.h"

// Sets default values
AObstacle::AObstacle()
{
    // Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
    PrimaryActorTick.bCanEverTick = true;

    // Create the Static Mesh Component
    ObstacleMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ObstacleMesh"));
    RootComponent = ObstacleMesh;
}

// Called when the game starts or when spawned
void AObstacle::BeginPlay()
{
    Super::BeginPlay();
}

// Called every frame
void AObstacle::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);
}

// Function to set the obstacle type
void AObstacle::SetObstacleType(EObstacleType NewType)
{
    ObstacleType = static_cast<uint8>(NewType); // Cast to uint8

    // Load and set the appropriate mesh based on the obstacle type
    if (ObstacleType == static_cast<uint8>(EObstacleType::Mountain)) // Compare with uint8 value
    {
        // Load and set the mountain mesh
        static ConstructorHelpers::FObjectFinder<UStaticMesh> MountainMeshAsset(TEXT("StaticMesh'/Game/Meshes/Mountain.Mountain'"));
        if (MountainMeshAsset.Succeeded())
        {
            ObstacleMesh->SetStaticMesh(MountainMeshAsset.Object);
        }
    }
    else if (ObstacleType == static_cast<uint8>(EObstacleType::Tree)) // Compare with uint8 value
    {
        // Load and set the tree mesh
        static ConstructorHelpers::FObjectFinder<UStaticMesh> TreeMeshAsset(TEXT("StaticMesh'/Game/Meshes/Tree.Tree'"));
        if (TreeMeshAsset.Succeeded())
        {
            ObstacleMesh->SetStaticMesh(TreeMeshAsset.Object);
        }
    }
}