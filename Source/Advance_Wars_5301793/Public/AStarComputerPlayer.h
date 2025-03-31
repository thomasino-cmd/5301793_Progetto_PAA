// AStarComputerPlayer.h
#pragma once

#include "CoreMinimal.h"
#include "ComputerPlayer.h"
#include "AStarComputerPlayer.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API AAStarComputerPlayer : public AComputerPlayer
{
    GENERATED_BODY()

public:

   
    void MoveBrawler() ;
    

    void MoveSniper() ;

protected:
    ATile* FindOptimalTargetTile(AActor* Unit, TArray<ATile*>& ReachableTiles);

    TArray<ATile*> AStarPathfinding(ATile* StartTile, ATile* TargetTile);
    
    float HeuristicCost(ATile* A, ATile* B);
};