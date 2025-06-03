// Copyright 2021-2023, Athian Games. All Rights Reserved. 


#include "MySQLDBConnectionActor.h"


// Sets default values
AMySQLDBConnectionActor::AMySQLDBConnectionActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	bIsConnectionBusy = false;

	CopyDLL(TEXT("mysqlcppconn-9-vs14.dll"));
	CopyDLL(TEXT("libcrypto-1_1-x64.dll"));
	CopyDLL(TEXT("libssl-1_1-x64.dll"));
}

// Called when the game starts or when spawned
void AMySQLDBConnectionActor::BeginPlay()
{
	CloseAllConnections();
	Super::BeginPlay();

}

FCriticalSection Mutex;

// Called every frame
void AMySQLDBConnectionActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Mutex.Lock(); // Lock access to shared resources
	// Clean up finished tasks
	CleanUpFinishedTasks<OpenMySQLConnectionTask>(OpenConnectionTasks);
	CleanUpFinishedTasks<UpdateMySQLQueryAsyncTask>(UpdateQueryTasks);
	CleanUpFinishedTasks<SelectMySQLQueryAsyncTask>(SelectQueryTasks);
	CleanUpFinishedTasks<UpdateMySQLImageAsyncTask>(UpdateImageQueryTasks);
	CleanUpFinishedTasks<SelectMySQLImageAsyncTask>(SelectImageQueryTasks);
	Mutex.Unlock(); // Unlock access to shared resources
	
	// Update the busy state
	bIsConnectionBusy = OpenConnectionTasks.Num() > 0
		|| UpdateQueryTasks.Num() > 0
		|| SelectQueryTasks.Num() > 0
		|| UpdateImageQueryTasks.Num() > 0
		|| SelectImageQueryTasks.Num() > 0;

	if (!bIsConnectionBusy)
	{
		for (auto& entry : ConnectionToNextQueryIDMap)
		{
			entry.Value = 0;
		}
	}

}

void AMySQLDBConnectionActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	// Ensure all UpdateQueryTasks have completed
	for(const auto& Task : UpdateQueryTasks)
	{
		if(Task && !Task->IsDone())
		{
			Task->EnsureCompletion();
		}
	}
	UpdateQueryTasks.Empty();

	// Ensure all SelectQueryTasks have completed
	for(const auto& Task : SelectQueryTasks)
	{
		if(Task && !Task->IsDone())
		{
			Task->EnsureCompletion();
		}
	}
	SelectQueryTasks.Empty();

	// Now you can safely close all connections
	CloseAllConnections();

	Super::EndPlay(EndPlayReason);
}


int32 AMySQLDBConnectionActor::GenerateQueryID(int32 ConnectionID)
{
	if (!ConnectionToNextQueryIDMap.Contains(ConnectionID))
	{
		ConnectionToNextQueryIDMap.Add(ConnectionID, 0);
	}

	const int32 QueryID = ConnectionToNextQueryIDMap[ConnectionID];
	ConnectionToNextQueryIDMap[ConnectionID]++;
	return QueryID;
}

int32 AMySQLDBConnectionActor::GetLastQueryID(int32 ConnectionID)
{
	int32 QueryID = -1;
	if (ConnectionToNextQueryIDMap.Contains(ConnectionID))
	{
		QueryID = ConnectionToNextQueryIDMap[ConnectionID];
	}

	return QueryID;

}

UMySQLDBConnector* AMySQLDBConnectionActor::GetConnector(int32 ConnectionID)
{
	if (UMySQLDBConnector** ConnectorPtr = SQLConnectors.Find(ConnectionID))
	{
		if (UMySQLDBConnector* CurrentConnector = *ConnectorPtr)
		{

			return CurrentConnector;
		}
	}

	return nullptr;
}

void AMySQLDBConnectionActor::CloseConnection(int32 ConnectionID)
{
	FQueryTaskData CloseConnectionTask;
	CloseConnectionTask.ConnectionID = ConnectionID;
	CloseConnectionTask.QueryType = EQueryType::Close;
	QueryTaskQueue.Add(CloseConnectionTask);

	if (!bIsQueryTaskRunning)
	{
		ExecuteNextQueryTask();
	}
}

void AMySQLDBConnectionActor::CloseAllConnections()
{
	TArray<int32> ConnectionKeys;
	SQLConnectors.GetKeys(ConnectionKeys);

	for (int iIndex = 0; iIndex < ConnectionKeys.Num(); iIndex++)
	{
		CloseConnection(ConnectionKeys[iIndex]);

	}

	SQLConnectors.Empty();
}



UMySQLDBConnector* AMySQLDBConnectionActor::CreateDBConnector(int32& ConnectionID)
{

	ConnectionID = SQLConnectors.Num();
	if (SQLConnectors.Num() > ConnectionID)
	{
		if (UMySQLDBConnector* NewConnector = SQLConnectors[ConnectionID])
		{
			NewConnector->CloseConnection(ConnectionID);
			NewConnector = nullptr;
			SQLConnectors.Remove(ConnectionID);
		}
	}

	UMySQLDBConnector* NewConnector = NewObject<UMySQLDBConnector>();
	SQLConnectors.Add(ConnectionID, NewConnector);
	return NewConnector;
}

void AMySQLDBConnectionActor::CopyDLL(FString DLLName)
{
	const FString Pluginpath = IPluginManager::Get().FindPlugin(TEXT("MySQL"))->GetBaseDir();
	const FString PluginDLLPath = FPaths::Combine(*Pluginpath, TEXT("Binaries"), TEXT("Win64"), DLLName);

	const FString ProjectDirectory = FPaths::ProjectDir();
	const FString ProjectDLLDirectory = FPaths::Combine(*ProjectDirectory, TEXT("Binaries"), TEXT("Win64"));

	const FString ProjectDLLPath = FPaths::Combine(*ProjectDLLDirectory, DLLName);

	if (!FPaths::DirectoryExists(*ProjectDLLDirectory))
	{
		FPlatformFileManager::Get().GetPlatformFile().CreateDirectoryTree(*ProjectDLLDirectory);
	}

	if (FPaths::FileExists(ProjectDLLPath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CopyFile(*PluginDLLPath, *ProjectDLLPath);
	}
	else if (FPaths::FileExists(PluginDLLPath))
	{
		FPlatformFileManager::Get().GetPlatformFile().CopyFile(*ProjectDLLPath, *PluginDLLPath);
	}
}

void AMySQLDBConnectionActor::ResetLastConnection()
{
	const int32 ConnectionID = SQLConnectors.Num() - 1;
	if(ConnectionID >= 0)
	{
		if(UMySQLDBConnector* CurrentConnector = SQLConnectors[ConnectionID])
		{
			CurrentConnector->ConditionalBeginDestroy();
			CurrentConnector = nullptr;
		}
		SQLConnectors.Remove(ConnectionID);
	}
}

void AMySQLDBConnectionActor::CreateNewConnection(FString Server, FString DBName, FString UserID, FString Password, int32 Port)
{
	int32 ConnectionID;
	UMySQLDBConnector* NewConnector = CreateDBConnector(ConnectionID);

	TArray<FMySQLOptionPair> MySQLOptions;
	if(MySQLOptionsAsset)
	{
		MySQLOptions = MySQLOptionsAsset->ConnectionOptions;
	}
	FAsyncTask<OpenMySQLConnectionTask>* OpenConnectionTask = StartAsyncTask<OpenMySQLConnectionTask>(this, ConnectionID, NewConnector, Server, DBName,
		UserID, Password, Port, MySQLOptions);
	OpenConnectionTasks.Add(OpenConnectionTask);

}

void AMySQLDBConnectionActor::ExecuteNextQueryTask()
{
	if(IsValidLowLevel())
	{
		if(QueryTaskQueue.Num() > 0)
		{
			FQueryTaskData& TaskData = QueryTaskQueue.Last();
			UMySQLDBConnector* CurrentConnector = GetConnector(TaskData.ConnectionID);
			if(CurrentConnector == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("CurrentConnector is null"));
				return;
			}

			bIsQueryTaskRunning = true;
			switch (TaskData.QueryType)
			{
			case EQueryType::Update:
				{
					FAsyncTask<UpdateMySQLQueryAsyncTask>* UpdateQueryTask = StartAsyncTask<UpdateMySQLQueryAsyncTask>(this, CurrentConnector, TaskData.ConnectionID, TaskData.QueryID, TaskData.Queries);
					if(UpdateQueryTask == nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("UpdateQueryTask is null"));
						return;
					}
					UpdateQueryTasks.Add(UpdateQueryTask);
				}
				break;
			case EQueryType::Select:
				{
				
					FAsyncTask<SelectMySQLQueryAsyncTask>* SelectQueryTask = StartAsyncTask<SelectMySQLQueryAsyncTask>(this, CurrentConnector, TaskData.ConnectionID, TaskData.QueryID, TaskData.Queries[0]);
					if(SelectQueryTask == nullptr)
					{
						UE_LOG(LogTemp, Warning, TEXT("SelectQueryTask is null"));
						return;
					}
					SelectQueryTasks.Add(SelectQueryTask);
				}
				break;
			case EQueryType::Close:
				{
					CurrentConnector->CloseConnection(TaskData.ConnectionID);
					SQLConnectors.Remove(TaskData.ConnectionID);
					ConnectionToNextQueryIDMap.Remove(TaskData.ConnectionID);
					bIsQueryTaskRunning = false;
				}
				break;
			case EQueryType::Endplay:
				{
					QueryTaskQueue.Empty();
					CloseAllConnections();
					Super::EndPlay(EEndPlayReason::Type::Quit);
				}
				break;
			default:
				break;
			}
			QueryTaskQueue.RemoveAt(QueryTaskQueue.Num() - 1);
		}
		else
		{
			bIsQueryTaskRunning = false;
		}
	}
}


void AMySQLDBConnectionActor::CreateTaskData(int32 ConnectionID, TArray<FString> Queries, EQueryType QueryType)
{
	// Create a struct with the query data and add it to the queue
	FQueryTaskData TaskData;
	TaskData.ConnectionID = ConnectionID;
	TaskData.QueryID = GenerateQueryID(ConnectionID);
	TaskData.Queries = Queries;
	TaskData.QueryType = QueryType;
	QueryTaskQueue.Add(TaskData);

	// If no task is currently running, execute the next task
	if (!bIsQueryTaskRunning)
	{
		ExecuteNextQueryTask();
	}
}

void AMySQLDBConnectionActor::UpdateDataFromQuery(int32 ConnectionID, FString Query)
{
	TArray<FString> Queries;
	Queries.Add(Query);
	CreateTaskData(ConnectionID, Queries, EQueryType::Update);
	
}

void AMySQLDBConnectionActor::UpdateDataFromMultipleQueries(int32 ConnectionID, TArray<FString> Queries)
{
	CreateTaskData(ConnectionID, Queries, EQueryType::Update);
}

void AMySQLDBConnectionActor::SelectDataFromQuery(int32 ConnectionID, FString Query)
{
	TArray<FString> Queries;
	Queries.Add(Query);
	CreateTaskData(ConnectionID, Queries, EQueryType::Select);
}

void AMySQLDBConnectionActor::UpdateImageFromPath(int32 ConnectionID, FString Query, FString UpdateParameter, int ParameterID, FString ImagePath)
{
	if (UMySQLDBConnector* CurrentConnector = GetConnector(ConnectionID))
	{
		int32 QueryID = GenerateQueryID(ConnectionID);
		FAsyncTask<UpdateMySQLImageAsyncTask>* UpdateImageQueryTask = StartAsyncTask<UpdateMySQLImageAsyncTask>(this, CurrentConnector, ConnectionID, QueryID, Query, UpdateParameter, ParameterID, ImagePath);
		UpdateImageQueryTasks.Add(UpdateImageQueryTask);

	}

}

bool AMySQLDBConnectionActor::UpdateImageFromTexture(int32 ConnectionID, FString Query, FString UpdateParameter, int ParameterID, UTexture2D* Texture)
{

	if (Texture)
	{
		FString TexturePath = FPaths::Combine(FPaths::ProjectDir(), TEXT("OutputImage.png"));
		TexturePath = IFileManager::Get().ConvertToAbsolutePathForExternalAppForRead(*TexturePath);

		if (UMySQLBPLibrary::SaveTextureToPath(Texture, TexturePath))
		{
			UpdateImageFromPath(ConnectionID, Query, UpdateParameter, ParameterID, TexturePath);
			return true;
		}

	}

	return false;
}

void AMySQLDBConnectionActor::SelectImageFromQuery(int32 ConnectionID, FString Query)
{
	if (UMySQLDBConnector* CurrentConnector = GetConnector(ConnectionID))
	{
		int32 QueryID = GenerateQueryID(ConnectionID);
		FAsyncTask<SelectMySQLImageAsyncTask>* SelectImageQueryTask = StartAsyncTask<SelectMySQLImageAsyncTask>(this, CurrentConnector, ConnectionID, QueryID, Query);
		SelectImageQueryTasks.Add(SelectImageQueryTask);

	}
}