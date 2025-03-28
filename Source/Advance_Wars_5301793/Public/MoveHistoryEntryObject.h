// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MoveHistoryEntryObject.generated.h"

/**
 * 
 */
UCLASS()
class ADVANCE_WARS_5301793_API UMoveHistoryEntryObject : public UObject
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly)
	FString DisplayString;
	
	UPROPERTY(meta = (BindWidget))
	class UTextBlock* EntryText;

	UFUNCTION(BlueprintCallable)
	void SetupEntry(const FString& Text);
};
