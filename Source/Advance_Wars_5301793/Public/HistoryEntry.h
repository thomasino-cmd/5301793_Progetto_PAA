// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h" 
#include "Blueprint/IUserObjectListEntry.h"
#include "HistoryEntry.generated.h"

/**
 * 
 */
UCLASS(Blueprintable) 
class ADVANCE_WARS_5301793_API UHistoryEntry : public UUserWidget, public IUserObjectListEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* EntryText;
	
	void SetupEntry(const FString& Text);

	virtual void NativeOnListItemObjectSet(UObject* ListItemObject) override;
};
