#pragma once

#include "CoreMinimal.h"
#include "Tile.h"
#include "GameFramework/Actor.h"
#include "Obstacle.h"
#include "GameField.generated.h"


// Macro declaration for a dynamic multicast delegate
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnReset);

UCLASS()
class ADVANCE_WARS_5301793_API AGameField : public AActor
{
	GENERATED_BODY()

public:
	UPROPERTY(Transient)
	TArray<ATile*> TileArray;

	static const int32 NOT_ASSIGNED = -1;

	// BlueprintAssignable Usable with Multicast Delegates only. Exposes the property for assigning in Blueprints.
    // Declare a variable of type FOnReset (delegate)
	 
	UPROPERTY(BlueprintAssignable)
	FOnReset OnResetEvent;

	// Sets default values for this actor's properties
	AGameField();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Generate the game field with tiles
	UFUNCTION(BlueprintCallable)
	void GenerateField();

	UFUNCTION(BlueprintCallable)
	void SpawnRandomObstacles();

	FVector GetRelativeLocationByXYPosition(const int32 InX, const int32 InY) const;

	UFUNCTION(BlueprintCallable)
	void ResetField();

	// Get a tile at the specified grid position
	UFUNCTION(BlueprintCallable)
	ATile* GetTile(int32 X, int32 Y) const;

	// Get the size of the game field (number of tiles in each dimension)
	UFUNCTION(BlueprintCallable)
	int32 GetSize() const;

	// Set the size of the game field (number of tiles in each dimension)
	UFUNCTION(BlueprintCallable)
	void SetSize(int32 NewSize);

	// Get the tile size in world units
	UFUNCTION(BlueprintCallable)
	float GetTileSize() const;


	UFUNCTION(BlueprintCallable)
	void SetGridCellOccupied(const FVector2D& GridPosition, int32 PlayerNumber);

	// Set the tile size in world units
	UFUNCTION(BlueprintCallable)
	void SetTileSize(float NewTileSize);

	// Get the distance between tiles in world units
	UFUNCTION(BlueprintCallable)
	float GetTileSpacing() const;

	// Set the distance between tiles in world units
	UFUNCTION(BlueprintCallable)
	void SetTileSpacing(float NewTileSpacing);

	// Size of the game field (number of tiles in each dimension)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Field")
	int32 Size;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Field")
	float CellPadding;


	// Tile size in world units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Field")
	float TileSize;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AObstacle> Tree1Class;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AObstacle> Tree2Class;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AObstacle> MountainClass;


protected:
	// Called when an instance of this class is placed (in editor) or spawned
	virtual void OnConstruction(const FTransform& Transform) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float NextCellPositionMultiplier;


	// Distance between tiles in world units
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game Field")
	float TileSpacing;

	// TMap to store the tiles, mapping FVector2D grid positions to ATile pointers
	UPROPERTY(Transient)
	TMap<FVector2D, ATile*> TileMap;

	UPROPERTY(EditDefaultsOnly, Category = "Game Field")
	TSubclassOf<ATile> TileClass;



};