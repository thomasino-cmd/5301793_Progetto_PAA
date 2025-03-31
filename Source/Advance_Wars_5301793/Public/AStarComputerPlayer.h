// AStarComputerPlayer.h
#pragma once

#include "CoreMinimal.h"
#include "ComputerPlayer.h"
#include "AStarComputerPlayer.generated.h"

UCLASS()
class ADVANCE_WARS_5301793_API AAStarComputerPlayer : public AComputerPlayer              //public APawn, public IPlayerInterface      // invece che public ComputerPlayer
{
    GENERATED_BODY()

public:

    void OnTurn();
   
    void MoveBrawler() ;

    ATile* FindTileToGetCloserToEnemy(AAW_Brawler* Brawler);

    bool CanBrawlerAttack(AAW_Brawler* Brawler);
    
    void MoveSniper() ;

    bool CanSniperAttack(AAW_Sniper* Sniper);

    ATile* FindNearestTileToEnemy(AAW_Sniper* Sniper);

    ATile* FindOptimalAttackTile(AActor* Unit);

    void ExecuteAttack(AActor* AttackingUnit);

    void MakeMove() ;

private:
    void ContinueWithNextUnit_AStar();

    FTimerHandle AStarBrawlerTimerHandle;
    FTimerHandle AStarSniperTimerHandle;


protected:
    ATile* FindOptimalTargetTile(AActor* Unit, TArray<ATile*>& ReachableTiles);

    TArray<ATile*> AStarPathfinding(ATile* StartTile, ATile* TargetTile);
    
    float HeuristicCost(ATile* A, ATile* B);
};