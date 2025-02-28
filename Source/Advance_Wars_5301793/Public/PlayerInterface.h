#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UPlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 *
 */
class ADVANCE_WARS_5301793_API IPlayerInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	// Player's ID (e.g., 0 for human, 1 for AI)
	int32 PlayerId;

	// Called when it's the player's turn
	virtual void OnTurn() {};

	// Called when the player wins the game
	virtual void OnWin() {};

	// Called when the player loses the game
	virtual void OnLose() {};
};