// Fill out your copyright notice in the Description page of Project Settings.
/*
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Tile.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API ATile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATile();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
*/

// Fill out your copyright notice in the Description page of Project Settings.

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
	int32 GetPlayerOwner() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Scene component for the tile
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Scene;

	// Static mesh component for the tile's visual representation
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* StaticMeshComponent;

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
