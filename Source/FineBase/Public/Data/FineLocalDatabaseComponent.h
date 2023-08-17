// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineDatabaseRecord.h"
#include "SQLiteDatabaseConnection.h"
#include "SQLiteResultSet.h"
#include "GameFramework/Info.h"
#include "FineLocalDatabaseComponent.generated.h"

class FSQLiteResultSet;
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

	FDataBaseRecordSet* FetchRecord(const FString& Entity, const FName& Name);
	FDataBaseRecordSet* FetchList(const FString& Entity);

	template <typename T>
	TArray<T> FetchModelList(const FString& Entity)
	{
		const auto ResultSet = FetchList(Entity);
		if (ResultSet == nullptr)
		{
			return {};
		}
		TArray<T> OutList;
		for (FSQLiteResultSet::TIterator It(ResultSet); It; ++It)
		{
			T Data;
			Data.UpdateFromRecord(FFineDatabaseRecord::FromResultSet(static_cast<const FSQLiteResultSet*>(*It)));
			OutList.Add(Data);
		}
		return OutList;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	FFineDisplayData GetDisplayDataByName(const FName Name, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	FFineActorData GetActorDataByName(const FName Name, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	FFineCharacterData GetCharacterDataByName(const FName Name,
	                                          bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	FFineDatabaseRecord GetRecordByName(const FString Entity, const FName Name, bool& bSuccess);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	TArray<FFineDisplayData> GetDisplayDataList();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category="FineBase")
	TArray<FFineActorData> GetActorDataList();

	UFUNCTION(BlueprintCallable, BlueprintCallable, Category="FineBase")
	TArray<FFineCharacterData> GetCharacterDataList();

	UFUNCTION(BlueprintCallable, Category="FineBase")
	void UpdateRecord(const FString& Entity, const FString& Name, const FFineDatabaseRecord& Record);

	UFUNCTION(BlueprintCallable, Category="FineBase")
	void CreateRecord(const FString& Entity, const FString& Name, const FFineDatabaseRecord& Record);

	UFUNCTION(BlueprintCallable, Category="FineBase")
	void DeleteRecords(const FString& Entity, const FString& Name);
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineBase", meta = (AllowPrivateAccess = "true"))
	FString DatabasePath;
	FSQLiteDatabaseConnection Connection;

	bool OpenConnection();
	FDataBaseRecordSet* ExecuteDatabaseQuery(const FString& Query);
};
