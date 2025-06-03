// Copyright 2018-2023, Athian Games. All Rights Reserved. 

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PostgreSQLBPLibrary.h"
#include "PostgreSQLAsyncTasks.h"

#include "PostgreSQLDBConnectionActor.generated.h"


UCLASS()
class POSTGRESQL_API APostgreSQLDBConnectionActor : public AActor
{
	GENERATED_BODY()
	
private:

	FAsyncTask<OpenPostgresConnectionTask>*   OpenConnectionTaskPtr;
	FAsyncTask<UpdatePostgresQueryAsyncTask>* UpdateQueryAsyncTaskPtr;
	FAsyncTask<SelectPostgresQueryAsyncTask>* SelectQueryAsyncTaskPtr;
	FAsyncTask<UpdatePostgresImageAsyncTask>* UpdateImageAsyncTaskPtr;
	FAsyncTask<SelectPostgresImageAsyncTask>* SelectImageAsyncTaskPtr;

public:	
	// Sets default values for this actor's properties
	APostgreSQLDBConnectionActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:	

	UPROPERTY()
		bool bIsConnectionBusy;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

		/**
	* Creates a New Database Connection
	*
	* @param	Server          SQL Server Name
	* @param	DBName	        Initial Database Name to be connected to
	* @param	UserID	        SQL Server UserID. Not relevant while using Windows Authentication
	* @param	Password    	SQL Server Password. Not relevant while using Windows Authentication
	* @param	ExtraParam   	Additional Connection Parameter to be included
	*/
	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		void CreateNewConnection(FString Server, FString DBName, FString UserID, FString Password, TMap<FString, FString> ExtraParams);


	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		void CloseConnection();

	UFUNCTION(BlueprintImplementableEvent, Category = "PostgreSQL")
		void OnConnectionStateChanged(bool ConnectionStatus, const FString& ErrorMessage);

	/**
	* Executes a Query to the database
	*
	* @param	Query           Query which is to be executed to the database
	* @param	SuccessStatus	Returns the status of Query execution , or busy if the connection is already busy
	* @param	ErrorMessage	Returns the exception message thrown if the Query execution fails
	*/
	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		void UpdateDataFromQuery(FString Query);

	UFUNCTION(BlueprintImplementableEvent, Category = "PostgreSQL")
		void OnQueryUpdateStatusChanged(bool IsSuccessful, const FString& ErrorMessage);


	/**
	* Selects data from the database
	*
	* @param	Query           Select Query which selects data from the database
   */
	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		void SelectDataFromQuery(FString Query);

	UFUNCTION(BlueprintImplementableEvent, Category = "PostgreSQL")
		void OnQuerySelectStatusChanged(bool IsSuccessful, const FString& ErrorMessage, const TArray<FPostgreSQLDataTable>& ResultByColumn, 
			const TArray<FPostgreSQLDataRow>& ResultByRow);


		/**
	* Updates image to the database from the hard drive Asynchronously
	*
	* @param	Query           Update Query which inserts or updates image to the database
	* @param	UpdateParameter Value which has been passed as a parameter for updating the image in the previous input,
								without the @ symbol
	* @param	ParameterID      The Parameter Order ID, so that ImageParameter can be searched afor nd replaced with necessary value
	* @param	Texture        Texture that needs to be converted to image and updated in the SQL Server
	* @param	ErrorMessage	Returns the exception message thrown if the Texture is not successfully read
	*/
	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		bool UpdateImageFromTexture(FString Query, FString UpdateParameter, int ParameterID, UTexture2D* Texture);

	/**
	* Updates image to the database from the hard drive Asynchronously
	*
	* @param	Query           Update Query which inserts or updates image to the database
	* @param	UpdateParameter Value which has been passed as a parameter for updating the image in the previous input,
								without the @ symbol
	* @param	ParameterID      The Parameter Order ID, so that ImageParameter can be searched afor nd replaced with necessary value
	* @param	ImagePath       Path of the Image that needs to be updated in the SQL Server
	* @param	SuccessStatus	Returns the status of Query execution , or busy if the connection is already busy
	* @param	ErrorMessage	Returns the exception message thrown if the Query execution fails
	*/
	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		void UpdateImageFromPath(FString Query, FString UpdateParameter, int ParameterID, FString ImagePath);


	UFUNCTION(BlueprintImplementableEvent, Category = "PostgreSQL")
		void OnImageUpdateStatusChanged(bool IsSuccessful, const FString& ErrorMessage);

	UFUNCTION(BlueprintImplementableEvent, Category = "PostgreSQL")
		void OnConnectionClosed();
	/**
	* Selects image from the database and returns Texture2D format of the selected image
	*
	* @param	Query           Select Query which fetches a single image from the database
	* @param	SelectParameter  The Image Parameter Name
	*/
	UFUNCTION(BlueprintCallable, Category = "PostgreSQL")
		void SelectImageFromQuery(FString Query, FString SelectParameter, int32 ParameterID);

	UFUNCTION(BlueprintImplementableEvent, Category = "PostgreSQL")
		void OnImageSelectStatusChanged(bool IsSuccessful, const FString& ErrorMessage, UTexture2D* SelectedTexture);



};
