// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Delegates/Delegate.h"

#include "AW_Delegates.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnMoveCompleted);

UCLASS()
class ADVANCE_WARS_5301793_API AAW_Delegates : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAW_Delegates();
	FOnMoveCompleted OnMoveCompleted;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
