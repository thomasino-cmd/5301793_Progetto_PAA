// Fill out your copyright notice in the Description page of Project Settings.


#include "ConeActor.h"

// Sets default values
AConeActor::AConeActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	Scene = CreateDefaultSubobject<USceneComponent>(TEXT("Scene"));
	ConeMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));

	Scene->SetupAttachment(GetRootComponent());
	ConeMesh->SetupAttachment(Scene);

	Speed = 50.f;
	Max = 200.f;
}

// Called when the game starts or when spawned
void AConeActor::BeginPlay()
{
	Super::BeginPlay();
	FVector Location = GetActorLocation();
	CurrentZ = Location.Z;
	MinHeight = Location.Z;
	MaxHeight = Location.Z + Max; 
	bIsUp = true; 
	
}

// Called every frame
void AConeActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	float Val = DeltaTime * Speed;
	if (bIsUp)
	{
		CurrentZ = CurrentZ + Val; 
	}
	else {
		CurrentZ = CurrentZ - Val; 
	}
	FVector Location = GetActorLocation();
	Location.Z = CurrentZ; 

	SetActorLocation(Location);

	if (CurrentZ > MaxHeight)
	{
		bIsUp = false;
	}
	else if (CurrentZ < MinHeight) {
		bIsUp = true;
	}
}

