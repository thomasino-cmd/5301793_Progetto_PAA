
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UENUM()
enum class ETileStatus : uint8
{
	EMPTY  UMETA(DisplayName = "Empty"),
	OBSTACLE UMETA(DisplayName = "Obstacle"),
	OCCUPIED UMETA(DisplayName = "Occupied")
};

class IAW_BaseSoldier;

UCLASS()
class ADVANCE_WARS_5301793_API ATile : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ATile();

	// Set the status of the tile
	void SetTileStatus(const int32 PlayerOwner,const ETileStatus NewStatus);

	// Get the status of the tile
	ETileStatus GetTileStatus() const;

	// Set the unit on the tile
	void SetUnit(AActor* NewUnit);

	// Get the unit on the tile
	AActor* GetUnit() const;

	// Set the grid position of the tile
	void SetGridPosition(const double X, const double Y);

	// Get the grid position of the tile
	FVector2D GetGridPosition() const;

	// Set the player owner of the tile
	void SetPlayerOwner(int32 NewPlayerOwner);

	// Get the player owner of the tile
	int32 GetTileOwner() const;


	UFUNCTION(BlueprintCallable, Category = "Tile")
	void Highlight(bool ShouldHighlight);

	// Metodo per ottenere il componente Static Mesh
	UFUNCTION(BlueprintCallable, Category = "Tile")
	UStaticMeshComponent* GetStaticMeshComponent() const;

	//UFUNCTION()
	//IAW_BaseSoldier* GetSoldier() const;

	UFUNCTION(BlueprintCallable, Category = "Tile")
	FString GetGridCoordinatesAsString() const;



	void SetTileMaterial() const;

	FString GetTileMaterialPath() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Scene component for the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	// Static mesh component for the tile's visual representation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* NormalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* HighlightedMaterial;



	// Status of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Tile")
	ETileStatus Status;

	// Unit on the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	AActor* Unit;

	// Grid position of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	FVector2D GridPosition;

	// Player owner of the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Tile")
	int32 PlayerOwner;
};
