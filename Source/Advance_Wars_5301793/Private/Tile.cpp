#include "Tile.h"

// Sets default values
ATile::ATile()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create the scene component
	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	SetRootComponent(Scene); // Set the scene component as the root component

	// Create the static mesh component
	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(Scene); // Attach the static mesh component to the scene component

	StaticMeshComponent->SetMaterial(0, NormalMaterial);

	// Initialize the tile's properties
	
	Unit = nullptr;
	GridPosition = FVector2D(0, 0);
	PlayerOwner = -1;
	Status = ETileStatus::EMPTY;
}

// Called when the game starts or when spawned
void ATile::BeginPlay()
{
	Super::BeginPlay();

	
}

// Set the status of the tile
void ATile::SetTileStatus(const int32 TileOwner ,const ETileStatus NewStatus)
{
	PlayerOwner = TileOwner;
	Status = NewStatus;
}

// Get the status of the tile
ETileStatus ATile::GetTileStatus() const
{
	
	return Status;
}

// Set the unit on the tile
void ATile::SetUnit(AActor* NewUnit)
{
	Unit = NewUnit;
}

// Get the unit on the tile
AActor* ATile::GetUnit() const
{
	return Unit;
}

// Set the grid position of the tile
void ATile::SetGridPosition(const double X, const double Y)
{
	GridPosition = FVector2D(X, Y);
}

// Get the grid position of the tile
FVector2D ATile::GetGridPosition() const
{
	return GridPosition;
}

// Set the player owner of the tile
void ATile::SetPlayerOwner(int32 NewPlayerOwner)
{
	PlayerOwner = NewPlayerOwner;
}

// Get the player owner of the tile
int32 ATile::GetTileOwner() const
{
	return PlayerOwner;
}

void ATile::SetTileMaterial() const
{
	const FString MaterialPath = GetTileMaterialPath();

	UMaterialInterface* Material = LoadObject<UMaterialInterface>(
		nullptr,
		*MaterialPath
	);

	StaticMeshComponent->SetMaterial(0, Material);
}

FString ATile::GetTileMaterialPath() const
{
	const FString MaterialPath = "/Game/Materials/MI_";

	if (Status == ETileStatus::EMPTY)
	{
		const FVector2D Position = GetGridPosition();
		const int32 X = static_cast<int>(Position.X);
		const int32 Y = static_cast<int>(Position.Y);

		const FString Color = ((X + Y) % 2 == 0)
			? "DARK"
			: "LIGHT";

		return MaterialPath + Color;
	}

	return MaterialPath ;
}



void ATile::Highlight(bool ShouldHighlight)
{
	if (ShouldHighlight)
	{
		// Change material or color to highlight
		StaticMeshComponent->SetMaterial(0, HighlightedMaterial);
	}
	else
	{
		// Reset material or color
		StaticMeshComponent->SetMaterial(0, NormalMaterial);
	}
}