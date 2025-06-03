// Copyright 2018-2023, Athian Games. All Rights Reserved. 


#include "PostgreSQLDBConnectionActor.h"

// Sets default values
APostgreSQLDBConnectionActor::APostgreSQLDBConnectionActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsConnectionBusy = false;

}

// Called when the game starts or when spawned
void APostgreSQLDBConnectionActor::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APostgreSQLDBConnectionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void APostgreSQLDBConnectionActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UPostgreSQLBPLibrary::CloseConnection();
	bIsConnectionBusy = false;
	
	if (OpenConnectionTaskPtr && !OpenConnectionTaskPtr->IsIdle())
	{
		OpenConnectionTaskPtr->Cancel();

	}
	if (UpdateQueryAsyncTaskPtr && !UpdateQueryAsyncTaskPtr->IsIdle())
	{
		UpdateQueryAsyncTaskPtr->Cancel();

	}
	if (SelectQueryAsyncTaskPtr && !SelectQueryAsyncTaskPtr->IsIdle())
	{
		SelectQueryAsyncTaskPtr->Cancel();

	}
	if (UpdateImageAsyncTaskPtr && !UpdateImageAsyncTaskPtr->IsIdle())
	{
		UpdateImageAsyncTaskPtr->Cancel();

	}
	if (SelectImageAsyncTaskPtr && !SelectImageAsyncTaskPtr->IsIdle())
	{
		SelectImageAsyncTaskPtr->Cancel();

	}

	Super::EndPlay(EndPlayReason);
	
}


void APostgreSQLDBConnectionActor::CreateNewConnection(FString Server, FString DBName, FString UserID, FString Password, TMap<FString, FString> ExtraParams)
{
	if (!bIsConnectionBusy)
	{
		OpenConnectionTaskPtr = new FAsyncTask<OpenPostgresConnectionTask>(this, Server, DBName, UserID, Password, ExtraParams);
		OpenConnectionTaskPtr->StartBackgroundTask();
		
		bIsConnectionBusy = true;
	}
}


void APostgreSQLDBConnectionActor::UpdateDataFromQuery(FString Query)
{
	if (!bIsConnectionBusy)
	{

		UpdateQueryAsyncTaskPtr = new FAsyncTask<UpdatePostgresQueryAsyncTask>(this, Query);
		UpdateQueryAsyncTaskPtr->StartBackgroundTask();
		bIsConnectionBusy = true;
	}

}

void APostgreSQLDBConnectionActor::SelectDataFromQuery(FString Query)
{
	if (!bIsConnectionBusy)
	{

		SelectQueryAsyncTaskPtr = new FAsyncTask<SelectPostgresQueryAsyncTask>(this, Query);
		SelectQueryAsyncTaskPtr->StartBackgroundTask();
		bIsConnectionBusy = true;
	}

}

void APostgreSQLDBConnectionActor::CloseConnection()
{
	Async(EAsyncExecution::Thread, [this]()
	{
		UPostgreSQLBPLibrary::CloseConnection();
		AsyncTask(ENamedThreads::GameThread, [this]()
		{
			OnConnectionClosed();
		});
	});

	bIsConnectionBusy = false;
}


void APostgreSQLDBConnectionActor::UpdateImageFromPath(FString Query, FString UpdateParameter, int ParameterID, FString ImagePath)
{
	if (!bIsConnectionBusy)
	{
		UpdateImageAsyncTaskPtr = new FAsyncTask<UpdatePostgresImageAsyncTask>(this, Query, UpdateParameter, ParameterID, ImagePath);
		UpdateImageAsyncTaskPtr->StartBackgroundTask();
		bIsConnectionBusy = true;
	}

}

bool APostgreSQLDBConnectionActor::UpdateImageFromTexture(FString Query, FString UpdateParameter, int ParameterID, UTexture2D* Texture)
{

	if (Texture)
	{
		FString TexturePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("OutputImage.png"));
		TexturePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*TexturePath);

		if (bool IsTextureSaved = UPostgreSQLBPLibrary::SaveTextureToPath(Texture, TexturePath))
		{
			UpdateImageFromPath(Query, UpdateParameter, ParameterID, TexturePath);
			return true;
		}
		
	}
	
	return false;
	
}


void APostgreSQLDBConnectionActor::SelectImageFromQuery(FString Query, FString SelectParameter, int32 ParameterID)
{
	if (!bIsConnectionBusy)
	{
		SelectImageAsyncTaskPtr = new FAsyncTask<SelectPostgresImageAsyncTask>(this, Query, SelectParameter, ParameterID);
		SelectImageAsyncTaskPtr->StartBackgroundTask();
		bIsConnectionBusy = true;
	}

}