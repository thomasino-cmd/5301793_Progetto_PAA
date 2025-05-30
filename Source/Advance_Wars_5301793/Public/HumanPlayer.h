#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PlayerInterface.h"
#include "AWGameInstance.h"
#include "Camera/CameraComponent.h"
#include "Delegates/Delegate.h" 
#include "AW_Brawler.h"
#include "AW_Sniper.h"
#include "HumanPlayer.generated.h"


DECLARE_DELEGATE(FClickDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCoinFlipRequested);

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



	//TArray<AActor*> HumanUnits;

	//mi serve per passare a handletileclick l'unit� che si sta muovendo 
	AActor* SelectedUnitForMovement ;

	//dato che bindUobject per handleenemyunitclick non accetta argomenti 
	AActor* SelectedUnitForAttack; 

	FTimerHandle TimerBrawler;
	FTimerHandle TimerSniper;


	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bBrawlerMovedThisTurn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bSniperMovedThisTurn;


	//////////////////////////////////////////



	// Implement interface functions
	virtual void OnTurn() override;
	virtual void OnWin() override;
	virtual void OnLose() override;



	UFUNCTION(BlueprintCallable)
	void RequestCoinFlip();


	UPROPERTY(BlueprintAssignable)
	FOnCoinFlipRequested OnCoinFlipRequested;




	// Function to handle mouse clicks
	UFUNCTION()
	void OnClick();



	//UFUNCTION()
	//void SetWaitingForMoveInput(bool bWaiting, ATile* Tile, const TArray<ATile*>& Tiles);


	UFUNCTION()
	void SetWaitingForMoveInput(bool bWaiting, AActor* Unit);
	
	UFUNCTION()
	void HandleTileClick();

	void AttackPhase(IAW_BaseSoldier* SelectedUnit);

	void CheckAndEndTurn();


	
	UFUNCTION()
	void HandleFriendlyUnitClick(AActor* ClickedUnit);

	UFUNCTION()
	void HandleEnemyUnitClick();

	UFUNCTION()
	void SelectUnit(AActor* Unit);


	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	bool bWaitingForMoveInput;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Attack")
	bool bWaitingForAttackInput;

	bool bBrawlerAttackedThisTurn;
	bool bSniperAttackedThisTurn;


protected:
	// Flag to track player's turn
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Game State")
	bool bIsMyTurn;

	//UFUNCTION()
	//void MoveUnit(ATile* CurrentTile, ATile* TargetTile);



	////UFUNCTION()
	////void MoveUnitToNextStep(float MoveDuration);



	//UFUNCTION()
	//void AttackUnit();

	






	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	//TArray<ATile*> CurrentPath;

	//UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Movement")
	//int32 CurrentPathIndex;


};