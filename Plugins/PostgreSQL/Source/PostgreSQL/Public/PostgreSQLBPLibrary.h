// Copyright 2018-2023, Athian Games. All Rights Reserved. 

#pragma once


#include "CoreMinimal.h"
#include "Engine/Texture2D.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Modules/ModuleManager.h"
#include "ImageUtils.h"
#include "Interfaces/IPluginManager.h"
#include "Serialization/BufferArchive.h"
#include "IImageWrapperModule.h"
#include "IImageWrapper.h"

#include <list>
#include <iostream>
#include <codecvt>

#include "libpq-fe.h"

#include <string>
using namespace std;


#include "PostgreSQLBPLibrary.generated.h"

USTRUCT(BlueprintType, Category = "PostgreSQL|Tables")
struct FPostgreSQLDataTable
{
	GENERATED_BODY()
		
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLDataTable")
		FString ColumnName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLDataTable")
		TArray<FString> ColumnData;
};

USTRUCT(BlueprintType, Category = "PostgreSQL|Tables")
struct FPostgreSQLDataRow
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "SQLDataRow")
		TArray<FString> RowData;
};


/**
* Contains all the methods that are used to connect to the C# dll 
* which takes care of connecting to the PostgreSQL server and executing
* the given queries. These methods can be directly called from GetErrorMessage
* the Blueprints.

*/
UCLASS()
class POSTGRESQL_API UPostgreSQLBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()


	static char* GetCharfromFString(FString Query);
	static wchar_t* GetWCharfromChar(const char* Input);
	//static char* GetCharFromTextureData(UTexture2D *Texture, FString Path);
	//static UTexture2D* LoadTexturefromCharData(const char* ImageChar);
	static bool BeginConnection();
	static void EndConnection();

	static void GetErrorMessage(FString& ErrorMessage);

public:

	static bool SetConnectionProperties(FString Server, FString DBName, FString UserID, FString Password,
		TMap<FString, FString> ExtraParams, FString& ErrorMessage);
	static void SelectDataFromQuery(FString Query, bool& IsSuccessful, FString& ErrorMessage, TArray<FPostgreSQLDataTable>
		& ResultByColumn, TArray<FPostgreSQLDataRow>& ResultByRow);
	static void UpdateDataFromQuery(FString Query, bool& IsSuccessful, FString& ErrorMessage);

	static void CloseConnection();
	static void CreateImageWrapperModule();
	static char* GetRawImageFromPath(FString ImagePath);
	static bool SaveTextureToPath(UTexture2D* Texture, const FString Path);
	static void UpdateImageFromPath(FString Query, FString UpdateParameter, int ParameterID, FString ImagePath, bool& IsSuccessful, FString& ErrorMessage);
	static UTexture2D* SelectImageFromQuery(FString Query, FString SelectParameter, int ParameterID, bool& IsSuccessful, FString& ErrorMessage);

	static void FlushImageRenderingCommands();
	static void GetTexturePixels(UTexture2D* Texture, TArray<FColor>& OutPixels);
	static int GetRawImageSize(FString ImagePath);
};
