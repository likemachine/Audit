#pragma once
#include "CoreMinimal.h"
#include "ClassroomScheduleRow.generated.h"

USTRUCT(BlueprintType)
struct FClassroomScheduleRow
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadWrite)
    FString Subject;

    UPROPERTY(BlueprintReadWrite)
    FString Teacher;

    UPROPERTY(BlueprintReadWrite)
    FString StartTime;

    UPROPERTY(BlueprintReadWrite)
    FString EndTime;

    UPROPERTY(BlueprintReadWrite)
    FString Weekday;
};
