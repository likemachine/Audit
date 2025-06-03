// Copyright 2021-2023, Athian Games. All Rights Reserved. 


#include "MySQLAsyncTasks.h"
#include "MySQLDBConnectionActor.h"
#include "MySQLBPLibrary.h"
#include "Async/Async.h"

OpenMySQLConnectionTask::OpenMySQLConnectionTask(TWeakObjectPtr<AMySQLDBConnectionActor> dbConnectionActor, int32 connectionID,
	TWeakObjectPtr<UMySQLDBConnector> dbConnector, FString server, FString dBName, FString userID, FString password, int32 port, TArray<FMySQLOptionPair> options)
{
	Server = server;
	DBName = dBName;
	UserID = userID;
	Password = password;
	Port = port;
	CurrentDBConnectionActor = dbConnectionActor;
	MySQLDBConnector = dbConnector;
	ConnectionID = connectionID;
	MySQLOptions = options;
}

OpenMySQLConnectionTask::~OpenMySQLConnectionTask()
{

}


void OpenMySQLConnectionTask::DoWork()
{

	if (MySQLDBConnector.IsValid())
	{
		MySQLDBConnector->CloseConnection(ConnectionID);

		FString ErrorMessage;
		bool ConnectionStatus = MySQLDBConnector->CreateNewConnection(ConnectionID, Server, DBName, UserID, Password, Port, MySQLOptions, ErrorMessage);
		AsyncTask(ENamedThreads::GameThread, [this, ConnectionStatus, ErrorMessage]()
		{
			
			if (CurrentDBConnectionActor.IsValid())
			{
				CurrentDBConnectionActor->bIsConnectionBusy = false;
				if(!ConnectionStatus)
				{
					CurrentDBConnectionActor->ResetLastConnection();
				}
				CurrentDBConnectionActor->OnConnectionStateChanged(ConnectionStatus, ConnectionID, ErrorMessage);
			}
			
		});

	}
	
}



UpdateMySQLQueryAsyncTask::UpdateMySQLQueryAsyncTask(TWeakObjectPtr<AMySQLDBConnectionActor> dbConnectionActor, TWeakObjectPtr<UMySQLDBConnector> dbConnector, int32 connectionID, int32 queryID, TArray<FString> queries)
{
	Queries = queries;
	CurrentDBConnectionActor = dbConnectionActor;
	MySQLDBConnector = dbConnector;
	ConnectionID = connectionID;
	QueryID = queryID;
}

UpdateMySQLQueryAsyncTask::~UpdateMySQLQueryAsyncTask()
{

}

void UpdateMySQLQueryAsyncTask::DoWork()
{
	FString ErrorMessage;
	bool currentUpdateQueryStatus = false;
	
	if (MySQLDBConnector.IsValid() && MySQLDBConnector->IsValidLowLevel())
	{
		for (int iIndex = 0; iIndex < Queries.Num(); iIndex++)
		{
			MySQLDBConnector->UpdateDataFromQuery(ConnectionID, QueryID, Queries[iIndex], currentUpdateQueryStatus, ErrorMessage);
		}
	}

	AsyncTask(ENamedThreads::GameThread, [this, currentUpdateQueryStatus, ErrorMessage]()
	{
		if (CurrentDBConnectionActor.IsValid() && CurrentDBConnectionActor->IsValidLowLevel())
		{
			CurrentDBConnectionActor->bIsConnectionBusy = false;
			CurrentDBConnectionActor->OnQueryUpdateStatusChanged(ConnectionID, QueryID, currentUpdateQueryStatus, ErrorMessage);
			CurrentDBConnectionActor->ExecuteNextQueryTask();
		}
	});
}

SelectMySQLQueryAsyncTask::SelectMySQLQueryAsyncTask(TWeakObjectPtr<AMySQLDBConnectionActor> dbConnectionActor, TWeakObjectPtr<UMySQLDBConnector> dbConnector, int32 connectionID, int32 queryID, FString query)
{
	Query = query;
	CurrentDBConnectionActor = dbConnectionActor;
	MySQLDBConnector = dbConnector;
	ConnectionID = connectionID;
	QueryID = queryID;
}

SelectMySQLQueryAsyncTask::~SelectMySQLQueryAsyncTask()
{
	
}

void SelectMySQLQueryAsyncTask::DoWork()
{
	
	FString ErrorMessage;
	bool SelectQueryStatus;
	TArray<FMySQLDataTable> ResultByColumn;
	TArray<FMySQLDataRow> ResultByRow;

	if (MySQLDBConnector.IsValid())
	{
		MySQLDBConnector->SelectDataFromQuery(ConnectionID, Query, SelectQueryStatus, ErrorMessage, ResultByColumn, ResultByRow);
	}
	else
	{
		ErrorMessage = "InValid Connection";
		SelectQueryStatus = false;
	}

	AsyncTask(ENamedThreads::GameThread, [this, SelectQueryStatus, ErrorMessage, ResultByColumn, ResultByRow]()
	{
		if (CurrentDBConnectionActor.IsValid() && CurrentDBConnectionActor->IsValidLowLevel())
		{
			CurrentDBConnectionActor->bIsConnectionBusy = false;
			CurrentDBConnectionActor->OnQuerySelectStatusChanged(ConnectionID, QueryID, SelectQueryStatus, ErrorMessage, ResultByColumn, ResultByRow);
			CurrentDBConnectionActor->ExecuteNextQueryTask();
		}
	});
	
}


UpdateMySQLImageAsyncTask::UpdateMySQLImageAsyncTask(TWeakObjectPtr<AMySQLDBConnectionActor> dbConnectionActor, TWeakObjectPtr<UMySQLDBConnector> dbConnector, int32 connectionID, int32 queryID, FString query, FString updateParameter, int parameterID, FString imagePath)
{
	Query = query;
	UpdateParameter = updateParameter;
	ParameterID = parameterID;
	ImagePath = imagePath;
	CurrentDBConnectionActor = dbConnectionActor;
	MySQLDBConnector = dbConnector;
	ConnectionID = connectionID;
	QueryID = queryID;

}

UpdateMySQLImageAsyncTask::~UpdateMySQLImageAsyncTask()
{

}

void UpdateMySQLImageAsyncTask::DoWork()
{
	bool UpdateQueryStatus;
	FString ErrorMessage;

	if (MySQLDBConnector.IsValid())
	{
		 MySQLDBConnector->UpdateImageFromPath(ConnectionID, QueryID, Query, UpdateParameter, ParameterID, ImagePath, UpdateQueryStatus, ErrorMessage);
	}
	else
	{
		ErrorMessage = "InValid Connection";
		UpdateQueryStatus = false;
	}
	
	AsyncTask(ENamedThreads::GameThread, [this, UpdateQueryStatus, ErrorMessage]()
		{
			if (CurrentDBConnectionActor.IsValid() && CurrentDBConnectionActor->IsValidLowLevel())
			{
				CurrentDBConnectionActor->bIsConnectionBusy = false;
				CurrentDBConnectionActor->OnImageUpdateStatusChanged(ConnectionID, QueryID, UpdateQueryStatus, ErrorMessage);
			}

		});
}


SelectMySQLImageAsyncTask::SelectMySQLImageAsyncTask(TWeakObjectPtr<AMySQLDBConnectionActor> dbConnectionActor, TWeakObjectPtr<UMySQLDBConnector> dbConnector,
	int32 connectionID, int32 queryID, FString query)
{
	Query = query;
	CurrentDBConnectionActor = dbConnectionActor;
	MySQLDBConnector = dbConnector;
	ConnectionID = connectionID;
	QueryID = queryID;

}

SelectMySQLImageAsyncTask::~SelectMySQLImageAsyncTask()
{

}

void SelectMySQLImageAsyncTask::DoWork()
{
	bool SelectQueryStatus;
	FString ErrorMessage;
	UTexture2D* SelectedTexture = nullptr;

	if (MySQLDBConnector.IsValid())
	{
		SelectedTexture = MySQLDBConnector->SelectImageFromQuery(ConnectionID, QueryID, Query, SelectQueryStatus, ErrorMessage);
	}
	else
	{
		ErrorMessage = "InValid Connection";
		SelectQueryStatus = false;
	}
	
	AsyncTask(ENamedThreads::GameThread, [this, SelectQueryStatus, ErrorMessage, SelectedTexture]()
		{
			if (CurrentDBConnectionActor.IsValid() && CurrentDBConnectionActor->IsValidLowLevel())
			{
				CurrentDBConnectionActor->bIsConnectionBusy = false;
				CurrentDBConnectionActor->OnImageSelectStatusChanged(ConnectionID, QueryID, SelectQueryStatus, ErrorMessage, SelectedTexture);
			}
		});

}





