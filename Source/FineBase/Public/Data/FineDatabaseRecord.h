// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Database.h"
#include "UObject/Object.h"
#include "FineDatabaseRecord.generated.h"

class FSQLiteResultSet;
/**
 * A set of fields that can be used to represent a database record.
 */
USTRUCT(BlueprintType)
struct FINEBASE_API FFineDatabaseRecord 
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDatabaseRecord")
	TMap<FString, int64> BigIntFields;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDatabaseRecord")
	TMap<FString, int32> IntFields;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDatabaseRecord")
	TMap<FString, float> FloatFields;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDatabaseRecord")
	TMap<FString, FString> StringFields;

	static FFineDatabaseRecord FromResultSet(const FDataBaseRecordSet* ResultSet);
	void UpdateFromResultSet(const FDataBaseRecordSet* ResultSet);
};
