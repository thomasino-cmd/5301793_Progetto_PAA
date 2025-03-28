#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "MoveHistoryManager.generated.h"

UENUM()
enum class EMoveType : uint8
{
    Move,
    Attack
};

UCLASS(BlueprintType)
class ADVANCE_WARS_5301793_API UMoveEntry : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    FString PlayerID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    FString UnitID;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    EMoveType MoveType;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    FString FromCell;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    FString ToCell;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    FString TargetCell;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Move History")
    int32 Damage;
};

UCLASS()
class ADVANCE_WARS_5301793_API UMoveHistoryManager : public UObject
{
    GENERATED_BODY()

public:
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Move History")
    TArray<TObjectPtr<UMoveEntry>> MoveHistory;


    UFUNCTION(BlueprintCallable, Category = "Move History")
    void LogMove(const FString& PlayerID, const FString& UnitID, const FString& From, const FString& To);

    UFUNCTION(BlueprintCallable, Category = "Move History")
    void LogAttack(const FString& PlayerID, const FString& UnitID, const FString& Target, int32 Damage);
};