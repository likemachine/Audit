
#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

UCLASS()
class AUDIT_API UMyGameInstance : public UGameInstance
{
    GENERATED_BODY()

public:
    virtual void Init() override;

    void CopyDatabaseIfNeeded();

    UPROPERTY(BlueprintReadWrite, VisibleAnywhere, Category = "QR")
    FString LastScannedRoomCode;
};
