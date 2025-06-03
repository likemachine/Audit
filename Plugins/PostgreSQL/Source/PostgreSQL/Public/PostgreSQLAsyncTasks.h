// Copyright 2018-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "Async/AsyncWork.h"

#include "Engine/DataTable.h"


class APostgreSQLDBConnectionActor;

class POSTGRESQL_API OpenPostgresConnectionTask : public FNonAbandonableTask
{

	FString Server;
	FString DBName;
	FString UserID;
	FString Password;
	FString ExtraParam;
	APostgreSQLDBConnectionActor* CurrentDBConnectionActor;
	TMap<FString, FString> ExtraParams;


public:


	OpenPostgresConnectionTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString server, FString dBName, FString userID, FString password, TMap<FString, FString> extraParams);

	virtual ~OpenPostgresConnectionTask();
	virtual void DoWork();
	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(OpenConnectionTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};


class POSTGRESQL_API UpdatePostgresQueryAsyncTask : public FNonAbandonableTask
{

private:

	FString Query;
	APostgreSQLDBConnectionActor* CurrentDBConnectionActor;

public:


	UpdatePostgresQueryAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query);

	virtual ~UpdatePostgresQueryAsyncTask();
	virtual void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UpdateQueryAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};


class POSTGRESQL_API SelectPostgresQueryAsyncTask : public FNonAbandonableTask
{

private:

	FString Query;
	APostgreSQLDBConnectionActor* CurrentDBConnectionActor;

public:


	SelectPostgresQueryAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query);

	virtual ~SelectPostgresQueryAsyncTask();
	virtual void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(SelectQueryAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};


class POSTGRESQL_API UpdatePostgresImageAsyncTask : public FNonAbandonableTask
{

private:

	FString Query;
	APostgreSQLDBConnectionActor* CurrentDBConnectionActor;
	FString UpdateParameter;
	int ParameterID;
	FString ImagePath;

public:


	UpdatePostgresImageAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query, FString updateParameter, int parameterID, FString imagePath);

	virtual ~UpdatePostgresImageAsyncTask();
	virtual void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(UpdateImageAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};


class POSTGRESQL_API SelectPostgresImageAsyncTask : public FNonAbandonableTask
{

private:

	FString Query;
	APostgreSQLDBConnectionActor* CurrentDBConnectionActor;
	FString SelectParameter;
	int32 ParameterID;

public:


	SelectPostgresImageAsyncTask(APostgreSQLDBConnectionActor* dbConnectionActor, FString query, FString selectParameter, int32 parameterID);

	virtual ~SelectPostgresImageAsyncTask();
	virtual void DoWork();

	FORCEINLINE TStatId GetStatId() const
	{
		RETURN_QUICK_DECLARE_CYCLE_STAT(SelectImageAsyncTask, STATGROUP_ThreadPoolAsyncTasks);
	}

};




