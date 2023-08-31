// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Info.h"
#include "FineLocalDatabaseComponent.generated.h"

class FSQLiteResultSet;
class FDataBaseRecordSet;
class FSQLiteDatabaseConnection;
struct FFineDatabaseRecord;
struct FFineCharacterData;
struct FFineActorData;
struct FFineDisplayData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDatabaseConnected);

/**
 * Info actor to prepare the calls to sqlite database on begin play event.
 */
UCLASS(Blueprintable, BlueprintType, Category = "FineBase", meta = (BlueprintSpawnableComponent))
class FINEBASE_API UFineLocalDatabaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UFineLocalDatabaseComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	virtual FString GetDatabaseFilePath() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	FFineDatabaseRecord GetRecordByName(const FString Entity, const FName Name, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	TArray<FFineDatabaseRecord> GetRecords(const FString& Entity, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	TArray<FFineDatabaseRecord> FilterRecords(const FString& Entity, const FString& WhereClause, bool& bSuccess);

	FORCEINLINE void SetDatabasePath(const FString& Path)
	{
		DatabasePath = Path;
		OpenConnection();
	}

	UPROPERTY(BlueprintAssignable)
	FOnDatabaseConnected OnDatabaseConnected;

	UFUNCTION(BlueprintCallable)
	virtual bool OpenConnection();
	UFUNCTION(BlueprintCallable)
	virtual void CloseConnection();

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	TSharedPtr<FSQLiteDatabaseConnection> Connection;

	FORCEINLINE const FString& GetDatabaseAssetPath() const { return DatabasePath; }
	TArray<FFineDatabaseRecord> ExecuteQuery(const FString& Query, bool& bSuccess);

private:
	/// The location of database to connect.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineBase", meta = (AllowPrivateAccess = "true"))
	FString DatabasePath;
};
