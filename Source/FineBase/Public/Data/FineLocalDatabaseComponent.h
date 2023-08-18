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

/**
 * Info actor to prepare the calls to sqlite database on begin play event.
 */
UCLASS(Blueprintable, BlueprintType, Category = "FineBase", meta = (BlueprintSpawnableComponent))
class FINEBASE_API UFineLocalDatabaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	UFineLocalDatabaseComponent();
	FString GetDatabaseConnectionString() const;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	FFineDatabaseRecord GetRecordByName(const FString Entity, const FName Name, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	TArray<FFineDatabaseRecord> GetRecords(const FString& Entity, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	TArray<FFineDatabaseRecord> FilterRecords(const FString& Entity, const FString& WhereClause, bool& bSuccess);

	UFUNCTION(BlueprintCallable, Category="FineBase")
	bool UpdateRecordByName(const FString& Entity, const FString& Name, const FFineDatabaseRecord& Record);

	UFUNCTION(BlueprintCallable, Category="FineBase")
	bool CreateRecord(const FString& Entity, const FString& Name, const FFineDatabaseRecord& Record);

	UFUNCTION(BlueprintCallable, Category="FineBase")
	bool DeleteRecordByName(const FString& Entity, const FString& Name);

	UFUNCTION(BlueprintCallable, Category="FineBase")
	bool DeleteRecords(const FString& Entity, const FString& WhereClause);

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineBase", meta = (AllowPrivateAccess = "true"))
	FString DatabasePath;
	TSharedPtr<FSQLiteDatabaseConnection> Connection;

	bool OpenConnection();
	void CloseConnection();
	TArray<FFineDatabaseRecord> ExecuteQuery(const FString& Query, bool& bSuccess);
};
