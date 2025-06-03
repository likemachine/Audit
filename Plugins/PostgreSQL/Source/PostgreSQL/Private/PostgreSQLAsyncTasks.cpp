// Copyright 2018-2023, Athian Games. All Rights Reserved. 


#include "PostgreSQLAsyncTasks.h"
#include "PostgreSQLDBConnectionActor.h"
#include "PostgreSQLBPLibrary.h"
#include "Async/Async.h"

OpenPostgresConnectionTask::OpenPostgresConnectionTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString server, FString dBName, FString userID, FString password, TMap<FString, FString> extraParams)
{
	Server = server;
	DBName = dBName;
	UserID = userID;
	Password = password;
	ExtraParams = extraParams;
	ExtraParams = extraParams;
	CurrentDBConnectionActor = dbConnectionActor;

}

OpenPostgresConnectionTask::~OpenPostgresConnectionTask()
{

}


void OpenPostgresConnectionTask::DoWork()
{
	FString ErrorMessage;
	bool ConnectionStatus = UPostgreSQLBPLibrary::SetConnectionProperties(Server, DBName, UserID, Password, ExtraParams, ErrorMessage);
	APostgreSQLDBConnectionActor* dbConnectionActor = CurrentDBConnectionActor;
	
	AsyncTask(ENamedThreads::GameThread, [dbConnectionActor, ConnectionStatus, ErrorMessage]()
		{
			if (dbConnectionActor && dbConnectionActor->IsValidLowLevel())
			{
				dbConnectionActor->bIsConnectionBusy = false;
				dbConnectionActor->OnConnectionStateChanged(ConnectionStatus, ErrorMessage);
			}
		});
}



UpdatePostgresQueryAsyncTask::UpdatePostgresQueryAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query)
{
	Query = query;
	CurrentDBConnectionActor = dbConnectionActor;
}

UpdatePostgresQueryAsyncTask::~UpdatePostgresQueryAsyncTask()
{

}

void UpdatePostgresQueryAsyncTask::DoWork()
{
	bool UpdateQueryStatus;
	FString ErrorMessage;
	UPostgreSQLBPLibrary::UpdateDataFromQuery(Query, UpdateQueryStatus, ErrorMessage);
	
	APostgreSQLDBConnectionActor* dbConnectionActor = CurrentDBConnectionActor;
	AsyncTask(ENamedThreads::GameThread, [dbConnectionActor, UpdateQueryStatus, ErrorMessage]()
		{
			if (dbConnectionActor && dbConnectionActor->IsValidLowLevel())
			{
				dbConnectionActor->bIsConnectionBusy = false;
				dbConnectionActor->OnQueryUpdateStatusChanged(UpdateQueryStatus, ErrorMessage);
			}
		});
}


SelectPostgresQueryAsyncTask::SelectPostgresQueryAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query)
{
	Query = query;
	CurrentDBConnectionActor = dbConnectionActor;
}

SelectPostgresQueryAsyncTask::~SelectPostgresQueryAsyncTask()
{

}

void SelectPostgresQueryAsyncTask::DoWork()
{
	bool UpdateQueryStatus;
	FString ErrorMessage;
	TArray<FPostgreSQLDataTable> ResultByColumn;
	TArray<FPostgreSQLDataRow> ResultByRow;
	UPostgreSQLBPLibrary::SelectDataFromQuery(Query, UpdateQueryStatus, ErrorMessage, ResultByColumn, ResultByRow);

	APostgreSQLDBConnectionActor* dbConnectionActor = CurrentDBConnectionActor;
	AsyncTask(ENamedThreads::GameThread, [dbConnectionActor, UpdateQueryStatus, ErrorMessage, ResultByColumn, ResultByRow]()
		{
			if (dbConnectionActor && dbConnectionActor->IsValidLowLevel())
			{
				dbConnectionActor->bIsConnectionBusy = false;
				dbConnectionActor->OnQuerySelectStatusChanged(UpdateQueryStatus, ErrorMessage, ResultByColumn, ResultByRow);
			}
		});
}


UpdatePostgresImageAsyncTask::UpdatePostgresImageAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query, FString updateParameter, int parameterID, FString imagePath)
{
	Query = query;
	UpdateParameter = updateParameter;
	ParameterID = parameterID;
	ImagePath = imagePath;
	CurrentDBConnectionActor = dbConnectionActor;
}

UpdatePostgresImageAsyncTask::~UpdatePostgresImageAsyncTask()
{

}

void UpdatePostgresImageAsyncTask::DoWork()
{
	bool UpdateQueryStatus;
	FString ErrorMessage;
	UPostgreSQLBPLibrary::UpdateImageFromPath(Query, UpdateParameter, ParameterID, ImagePath, UpdateQueryStatus, ErrorMessage);

	APostgreSQLDBConnectionActor* dbConnectionActor = CurrentDBConnectionActor;
	AsyncTask(ENamedThreads::GameThread, [dbConnectionActor, UpdateQueryStatus, ErrorMessage]()
		{
			if (dbConnectionActor && dbConnectionActor->IsValidLowLevel())
			{
				dbConnectionActor->bIsConnectionBusy = false;
				dbConnectionActor->OnImageUpdateStatusChanged(UpdateQueryStatus, ErrorMessage);
			}

		});
}


SelectPostgresImageAsyncTask::SelectPostgresImageAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query, FString selectParameter, int32 parameterID)
{
	Query = query;
	SelectParameter = selectParameter;
	CurrentDBConnectionActor = dbConnectionActor;
	ParameterID = parameterID;
}

SelectPostgresImageAsyncTask::~SelectPostgresImageAsyncTask()
{

}

void SelectPostgresImageAsyncTask::DoWork()
{
	bool SelectQueryStatus;
	FString ErrorMessage;
	UTexture2D* SelectedTexture = UPostgreSQLBPLibrary::SelectImageFromQuery(Query, SelectParameter, ParameterID, SelectQueryStatus, ErrorMessage);

	APostgreSQLDBConnectionActor* dbConnectionActor = CurrentDBConnectionActor;
	AsyncTask(ENamedThreads::GameThread, [dbConnectionActor, SelectQueryStatus, ErrorMessage, SelectedTexture]()
		{
			if (dbConnectionActor && dbConnectionActor->IsValidLowLevel())
			{
				dbConnectionActor->bIsConnectionBusy = false;
				dbConnectionActor->OnImageSelectStatusChanged(SelectQueryStatus, ErrorMessage, SelectedTexture);
			}
		});
}





