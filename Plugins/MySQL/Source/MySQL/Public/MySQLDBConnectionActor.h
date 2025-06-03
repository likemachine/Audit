// Copyright 2021-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MySQLBPLibrary.h"
#include "MySQLAsyncTasks.h"
#include "Interfaces/IPluginManager.h"
#include "MySQLDBConnector.h"

#include "MySQLDBConnectionActor.generated.h"

UENUM()
enum EQueryType
{
	Update,
	Select,
	Close,
	Endplay
};

USTRUCT()
struct FQueryTaskData
{
	GENERATED_BODY()
	
	int32 ConnectionID;
	int32 QueryID;
	TArray<FString> Queries;
		
	EQueryType QueryType; // Define an enumeration EQueryType with values like Select, Update, etc.
	// Add any other required parameters for the query

	friend bool operator==(const FQueryTaskData& lhs, const FQueryTaskData& rhs)
	{
		return lhs.ConnectionID == rhs.ConnectionID &&  lhs.QueryID == rhs.QueryID;
	}
};

UCLASS()
class MYSQL_API AMySQLDBConnectionActor : public AActor
{
	GENERATED_BODY()
	
	TMap<int32, int32> ConnectionToNextQueryIDMap;

template<typename TaskType, typename... Args>
FAsyncTask<TaskType>* StartAsyncTask(Args&&... args)
{
	TaskType Task(std::forward<Args>(args)...);
	FAsyncTask<TaskType>* AsyncTask = new FAsyncTask<TaskType>(std::move(Task));
	AsyncTask->StartBackgroundTask();
	return AsyncTask;
}

template<class T>
void CleanUpFinishedTasks(TArray<FAsyncTask<T>*> &TaskArray)
{
	for (int32 Index = 0; Index < TaskArray.Num(); ++Index)
	{
		FAsyncTask<T>* Task = TaskArray[Index];
		if (Task->IsDone())
		{
			delete Task;
			TaskArray.RemoveAt(Index--);
		}
	}
}
	
public:

	
	int32 GenerateQueryID(int32 ConnectionID);

	AMySQLDBConnectionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UMySQLDBConnector* GetConnector(int32 ConnectionID);

	TArray<FAsyncTask<OpenMySQLConnectionTask>*> OpenConnectionTasks;
	TArray<FAsyncTask<UpdateMySQLQueryAsyncTask>*> UpdateQueryTasks;
	TArray<FAsyncTask<SelectMySQLQueryAsyncTask>*> SelectQueryTasks;
	TArray<FAsyncTask<UpdateMySQLImageAsyncTask>*> UpdateImageQueryTasks;
	TArray<FAsyncTask<SelectMySQLImageAsyncTask>*> SelectImageQueryTasks;
	
private:

	

	// Declare a queue to store the pending query tasks
	TArray<FQueryTaskData> QueryTaskQueue;

	// Declare a boolean to indicate whether a query task is currently running
	bool bIsQueryTaskRunning;
	void CreateTaskData(int32 ConnectionID, TArray<FString> Queries, EQueryType QueryType);

	UMySQLDBConnector* CreateDBConnector(int32& ConnectionID);

	static void CopyDLL(FString DLLName);

public:	

	// Method to initiate execution of next query in the queue
	void ExecuteNextQueryTask();
	void ResetLastConnection();

	UPROPERTY()
		bool bIsConnectionBusy;

	UPROPERTY()
		bool bIsSelectQueryBusy;

	UPROPERTY()
		TMap<int32, UMySQLDBConnector*> SQLConnectors;

	FTimerHandle SelectDataTaskTimer;
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	/**
	* Creates a New Database Connection
	*/
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void CreateNewConnection(FString Server, FString DBName, FString UserID, FString Password, int32 Port);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="MySQLOptions")
	UMySQLConnectionOptions* MySQLOptionsAsset;

	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void CloseAllConnections();

	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void CloseConnection(int32 ConnectionID);

	UFUNCTION(BlueprintImplementableEvent, Category = "MySql Server")
		void OnConnectionStateChanged(bool ConnectionStatus, int32 ConnectionID, const FString& ErrorMessage);

	UFUNCTION(BlueprintPure, Category = "MySql Server")
	int32 GetLastQueryID(int32 ConnectionID);

	UFUNCTION(BlueprintPure, Category = "MySql Server")
	bool CheckIsQueryRunning()
	{
		return bIsQueryTaskRunning;
	}

	/**
	* Executes a Query to the database
	*/
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
	void UpdateDataFromQuery(int32 ConnectionID, FString Query);

	/**
	* Executes Multiple Queries Simultaneously to the database
	*/
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void UpdateDataFromMultipleQueries(int32 ConnectionID, TArray<FString> Queries);

	UFUNCTION(BlueprintImplementableEvent, Category = "MySql Server")
		void OnQueryUpdateStatusChanged(int32 ConnectionID, int32 QueryID, bool IsSuccessful, const FString& ErrorMessage);


	/**
	* Selects data from the database
   */
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void SelectDataFromQuery(int32 ConnectionID, FString Query);

	UFUNCTION(BlueprintImplementableEvent, Category = "MySql Server")
		void OnQuerySelectStatusChanged(int32 ConnectionID, int32 QueryID, bool IsSuccessful, const FString& ErrorMessage, const TArray<FMySQLDataTable>& ResultByColumn, 
			const TArray<FMySQLDataRow>& ResultByRow);


	/**
		* Updates image to the database from the hard drive Asynchronously
	*/
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		bool UpdateImageFromTexture(int32 ConnectionID, FString Query, FString UpdateParameter, int ParameterID, UTexture2D* Texture);

	/**
	* Updates image to the database from the hard drive Asynchronously
	*/
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void UpdateImageFromPath(int32 ConnectionID, FString Query, FString UpdateParameter, int ParameterID, FString ImagePath);


	UFUNCTION(BlueprintImplementableEvent, Category = "MySql Server")
		void OnImageUpdateStatusChanged(int32 ConnectionID, int32 QueryID, bool IsSuccessful, const FString& ErrorMessage);

	/**
	* Selects image from the database and returns Texture2D format of the selected image
	*/
	UFUNCTION(BlueprintCallable, Category = "MySql Server")
		void SelectImageFromQuery(int32 ConnectionID, FString Query);

	UFUNCTION(BlueprintImplementableEvent, Category = "MySql Server")
		void OnImageSelectStatusChanged(int32 ConnectionID, int32 QueryID, bool IsSuccessful, const FString& ErrorMessage, UTexture2D* SelectedTexture);
};
