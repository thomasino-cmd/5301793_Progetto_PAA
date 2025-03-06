#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInterface.h"
#include "AWGameInstance.h"
#include "Camera/CameraComponent.h"
#include "HumanPlayer.generated.h"

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


	// Implement interface functions
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;

	// Function to handle mouse clicks
	UFUNCTION()
	void OnClick();

protected:
	// Flag to track player's turn
	bool bIsMyTurn;

	UFUNCTION()
	void MoveUnit(ATile* TargetTile);

	UFUNCTION()
	void AttackUnit(AComputerPlayer* TargetUnit);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Units")
	AActor* SelectedUnit;
};