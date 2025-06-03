#include "MyGameInstance.h"
#include "Misc/Paths.h"
#include "HAL/PlatformFilemanager.h"

void UMyGameInstance::Init()
{
    Super::Init();

    UE_LOG(LogTemp, Log, TEXT("GameInstance инициализирован."));
    CopyDatabaseIfNeeded();
}

void UMyGameInstance::CopyDatabaseIfNeeded()
{
    FString SourcePath = FPaths::ProjectContentDir() + "Movies/TestDB.db";
    FString DestPath = FPaths::ProjectPersistentDownloadDir() + "TestDB.db";

    UE_LOG(LogTemp, Log, TEXT("Попытка копирования базы данных"));
    UE_LOG(LogTemp, Log, TEXT("Source: %s"), *SourcePath);
    UE_LOG(LogTemp, Log, TEXT("Destination: %s"), *DestPath);

    IPlatformFile& PlatformFile = FPlatformFileManager::Get().GetPlatformFile();

    if (!FPaths::FileExists(SourcePath))
    {
        UE_LOG(LogTemp, Error, TEXT("Исходная база данных не найдена в Movies/: %s"), *SourcePath);
        return;
    }

    if (!FPaths::FileExists(DestPath))
    {
        if (PlatformFile.CopyFile(*DestPath, *SourcePath))
        {
            UE_LOG(LogTemp, Log, TEXT("База данных скопирована в: %s"), *DestPath);
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("Не удалось скопировать базу данных в: %s"), *DestPath);
        }
    }
    else
    {
        UE_LOG(LogTemp, Log, TEXT("База уже существует: %s"), *DestPath);
    }
}
