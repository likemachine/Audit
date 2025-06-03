
#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "ClassroomScheduleRow.h"
#include "SQLiteScheduleLibrary.generated.h"

UCLASS()
class AUDIT_API USQLiteScheduleLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category = "SQLite")
    static TArray<FClassroomScheduleRow> GetScheduleForRoom(const FString& RoomCode);
};
