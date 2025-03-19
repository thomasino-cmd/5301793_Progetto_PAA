#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInterface.h"
#include "AWGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Delegates/Delegate.h" 
#include "HumanPlayer.generated.h"


DECLARE_DELEGATE(FClickDelegate);

UCLASS()
class ADVANCE_WARS_5301793_API AHumanPlayer : public APawn, public IPlayerInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AHumanPlayer();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// Camera component attached to the player pawn
	//NO : UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* Camera;


	//GameIstance reference
	UAWGameInstance* GameIstance;

	FClickDelegate OnClickAction;




	// Implement interface functions
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;


public:
	void SetCurrentTile(ATile* Tile);
	void SetReachableTiles(TArray<ATile*> Tiles);

	TArray<ATile*> GetReachableTiles() const;




	// Function to handle mouse clicks
	UFUNCTION()
	void OnClick();



	UFUNCTION()
	void SetWaitingForMoveInput(bool bWaiting, ATile* Tile, const TArray<ATile*>& Tiles);

	UFUNCTION()
	void HandleTileClick();


	
	UFUNCTION()
	void HandleFriendlyUnitClick(AActor* ClickedUnit);

	UFUNCTION()
	void HandleEnemyUnitClick(AActor* ClickedEnemyUnit);

	UFUNCTION()
	void SelectUnit(AActor* Unit);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWaitingForMoveInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Units")
	AActor* SelectedUnit;

protected:
	// Flag to track player's turn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bIsMyTurn;

	UFUNCTION()
	void MoveUnit(ATile* CurrentTile, ATile* TargetTile);

	UFUNCTION()
	void MoveUnit_Tick();

	UFUNCTION()
	void AttackUnit();

	






	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<ATile*> CurrentPath;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	int32 CurrentPathIndex;


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	FTimerHandle MovementTimerHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MovementSpeed ; 

private:
	ATile* CurrentActorTile;
	TArray<ATile*> ReachableTiles;
};