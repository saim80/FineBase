// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineLocalDatabaseComponent.h"

#include "FineBase.h"
#include "SQLiteDatabaseConnection.h"
#include "Data/FineActorData.h"
#include "Data/FineCharacterData.h"
#include "Data/FineDatabaseRecord.h"
#include "Data/FineDisplayData.h"

UFineLocalDatabaseComponent::UFineLocalDatabaseComponent(): Super()
{
}

FString UFineLocalDatabaseComponent::GetDatabaseConnectionString() const
{
	// Convert the asset path to file path.
	// Get the game directory.
	const FString GameDir = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	// Check if the path has a prefix of /Game, then remove it.
	FString StrippedPathToDB = DatabasePath.StartsWith("/Game") ? DatabasePath.RightChop(5) : DatabasePath;
	// Check if the path has a prefix of /, then remove it.
	StrippedPathToDB = StrippedPathToDB.StartsWith("/") ? StrippedPathToDB.RightChop(1) : StrippedPathToDB;

	// Get the file path by using path class.
	const FString FilePath = FPaths::Combine(GameDir, StrippedPathToDB);

	return FilePath;
}

void UFineLocalDatabaseComponent::BeginPlay()
{
	Super::BeginPlay();

	OpenConnection();
}

void UFineLocalDatabaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Connection.Close();
	Super::EndPlay(EndPlayReason);
}

FDataBaseRecordSet* UFineLocalDatabaseComponent::FetchRecord(const FString& Entity, const FName& Name)
{
	const FString Query = FString::Printf(TEXT("SELECT * FROM %s WHERE Name = '%s';"), *Entity, *Name.ToString());
	return ExecuteDatabaseQuery(Query);
}

FDataBaseRecordSet* UFineLocalDatabaseComponent::FetchList(const FString& Entity)
{
	const FString Query = FString::Printf(TEXT("SELECT * FROM %s;"), *Entity);
	return ExecuteDatabaseQuery(Query);
}

bool UFineLocalDatabaseComponent::OpenConnection()
{
	const auto FilePath = GetDatabaseConnectionString();
	FB_LOG("Opening database at %s", *FilePath);

	if (!Connection.Open(*FilePath, TEXT(""), TEXT("")))
	{
		FB_ERROR("Failed to open database.");
		return false;
	}
	FB_LOG("Database opened successfully.");
	return true;
}

FDataBaseRecordSet* UFineLocalDatabaseComponent::ExecuteDatabaseQuery(const FString& Query)
{
	// Execute the query and get the result set.
	FSQLiteResultSet* ResultSet = nullptr;
	if (!Connection.Execute(*Query, ResultSet))
	{
		if (!Connection.GetLastError().IsEmpty())
		{
			FB_ERROR("SQLite query failed: %s %s", *Query, *Connection.GetLastError());
		}
		else
		{
			FB_ERROR("SQLite query failed: %s", *Query);
		}
		return nullptr;
	}
#if WITH_EDITOR
	FB_LOG("SQLite query succeeded: %s", *Query);
	TArray<FString> Names;
	// print the fetched result as verbose level log.
	for (const auto Name : ResultSet->GetColumnNames())
	{
		Names.Push(Name.ColumnName);
	}
	FB_VERBOSE("SQLite query result: %s", *FString::Join(Names, TEXT(", ")));

	// iterate the result set.
	for (FSQLiteResultSet::TIterator It(ResultSet); It; ++It)
	{
		const auto Row = *It;
		FString OutRow = "";
		// iterate the columns.
		for (const auto Name : ResultSet->GetColumnNames())
		{
			switch (Name.DataType)
			{
			case DBT_INT:
				OutRow.Append(FString::Printf(TEXT("%d, "), Row->GetInt(*Name.ColumnName)));
				break;
			case DBT_FLOAT:
				OutRow.Append(FString::Printf(TEXT("%f, "), Row->GetFloat(*Name.ColumnName)));
				break;
			case DBT_STRING:
				OutRow.Append(FString::Printf(TEXT("%s, "), *Row->GetString(*Name.ColumnName)));
				break;
			default:
				OutRow.Append(FString::Printf(TEXT("Unknown Value %i"), Name.DataType));
			}
		}

		FB_VERBOSE("SQLite query result: %s", *OutRow);
	}
#endif

	const FSQLiteResultSet::TIterator It(ResultSet);
	return *It;
}

FFineDisplayData UFineLocalDatabaseComponent::GetDisplayDataByName(const FName Name, bool& bSuccess)
{
	FFineDisplayData OutData;
	const auto Record = GetRecordByName(TEXT("Display"), Name, bSuccess);
	if (!bSuccess)
	{
		return OutData;
	}
	OutData.UpdateFromRecord(Record);
	return OutData;
}

FFineActorData UFineLocalDatabaseComponent::GetActorDataByName(const FName Name, bool& bSuccess)
{
	FFineActorData OutData;
	const auto Record = GetRecordByName(TEXT("Actor"), Name, bSuccess);
	if (!bSuccess)
	{
		return OutData;
	}
	OutData.UpdateFromRecord(Record);
	return OutData;
}

FFineCharacterData UFineLocalDatabaseComponent::GetCharacterDataByName(const FName Name, bool& bSuccess)
{
	FFineCharacterData OutData;
	const auto Record = GetRecordByName(TEXT("Character"), Name, bSuccess);
	if (!bSuccess)
	{
		return OutData;
	}
	OutData.UpdateFromRecord(Record);
	return OutData;
}

FFineDatabaseRecord UFineLocalDatabaseComponent::GetRecordByName(const FString Entity, const FName Name,
                                                                 bool& bSuccess)
{
	const auto ResultSet = FetchRecord(Entity, Name);
	if (ResultSet == nullptr)
	{
		bSuccess = false;
		return {};
	}
	bSuccess = true;
	const auto Record = FFineDatabaseRecord::FromResultSet(ResultSet);
	delete ResultSet;
	return Record;
}


TArray<FFineDisplayData> UFineLocalDatabaseComponent::GetDisplayDataList()
{
	return FetchModelList<FFineDisplayData>(TEXT("Display"));
}

TArray<FFineActorData> UFineLocalDatabaseComponent::GetActorDataList()
{
	return FetchModelList<FFineActorData>(TEXT("Actor"));
}

TArray<FFineCharacterData> UFineLocalDatabaseComponent::GetCharacterDataList()
{
	return FetchModelList<FFineCharacterData>(TEXT("Character"));
}

void UFineLocalDatabaseComponent::UpdateRecord(const FString& Entity, const FString& Name,
                                               const FFineDatabaseRecord& Record)
{
	// Create SQL query for update row for the given entity and record.
	FString Query = FString::Printf(TEXT("UPDATE %s SET "), *Entity);

	// For each string field in the record, add the field name and value to the query.
	for (const auto& Field : Record.StringFields)
	{
		Query += FString::Printf(TEXT("%s = '%s', "), *Field.Key, *Field.Value);
	}
	// For each int field in the record, add the field name and value to the query.
	for (const auto& Field : Record.IntFields)
	{
		Query += FString::Printf(TEXT("%s = %d, "), *Field.Key, Field.Value);
	}
	// For each big int field in the record, add the field name and value to the query.
	for (const auto& Field : Record.BigIntFields)
	{
		Query += FString::Printf(TEXT("%s = %lld, "), *Field.Key, Field.Value);
	}
	// For each float field in the record, add the field name and value to the query.
	for (const auto& Field : Record.FloatFields)
	{
		Query += FString::Printf(TEXT("%s = %f, "), *Field.Key, Field.Value);
	}
	// Remove the last comma and space.
	Query.RemoveAt(Query.Len() - 2, 2);
	// Add the where clause.
	Query += FString::Printf(TEXT(" WHERE Name = '%s'"), *Name);
	// Add semicolon to end the statement.
	Query += TEXT(";");
	// Execute the query.
	const auto Result = ExecuteDatabaseQuery(Query);
	// Check record count to see if the update was successful.
	if (Result != nullptr)
	{
		FB_VERBOSE("%s Updated.", *Name);
	}

	delete Result;
}

void UFineLocalDatabaseComponent::CreateRecord(const FString& Entity, const FString& Name,
                                               const FFineDatabaseRecord& Record)
{
	// Create SQL query for insert row for the given entity and record.
	FString Query = FString::Printf(TEXT("INSERT INTO %s ("), *Entity);

	// Add 'Name" as field.
	Query += TEXT("Name, ");

	// For each string field in the record, add the field name to the query.
	for (const auto& Field : Record.StringFields)
	{
		Query += FString::Printf(TEXT("%s, "), *Field.Key);
	}
	// For each int field in the record, add the field name to the query.
	for (const auto& Field : Record.IntFields)
	{
		Query += FString::Printf(TEXT("%s, "), *Field.Key);
	}
	// For each big int field in the record, add the field name to the query.
	for (const auto& Field : Record.BigIntFields)
	{
		Query += FString::Printf(TEXT("%s, "), *Field.Key);
	}
	// For each float field in the record, add the field name to the query.
	for (const auto& Field : Record.FloatFields)
	{
		Query += FString::Printf(TEXT("%s, "), *Field.Key);
	}
	// Remove the last comma and space.
	Query.RemoveAt(Query.Len() - 2, 2);
	// Add the values clause.
	Query += FString::Printf(TEXT(") VALUES ("));
	// Add the name value.
	Query += FString::Printf(TEXT("'%s', "), *Name);
	// For each string field in the record, add the field value to the query.
	for (const auto& Field : Record.StringFields)
	{
		Query += FString::Printf(TEXT("'%s', "), *Field.Value);
	}
	// For each int field in the record, add the field value to the query.
	for (const auto& Field : Record.IntFields)
	{
		Query += FString::Printf(TEXT("%d, "), Field.Value);
	}
	// For each big int field in the record, add the field value to the query.
	for (const auto& Field : Record.BigIntFields)
	{
		Query += FString::Printf(TEXT("%lld, "), Field.Value);
	}
	// For each float field in the record, add the field value to the query.
	for (const auto& Field : Record.FloatFields)
	{
		Query += FString::Printf(TEXT("%f, "), Field.Value);
	}
	// Remove the last comma and space.
	Query.RemoveAt(Query.Len() - 2, 2);
	// Add semicolon to end the statement.
	Query += TEXT(");");

	// Execute the query.
	const auto Result = ExecuteDatabaseQuery(Query);
	// Check record count to see if the update was successful.
	if (Result != nullptr)
	{
		FB_VERBOSE("%s Created.", *Name);
	}

	delete Result;
}

void UFineLocalDatabaseComponent::DeleteRecords(const FString& Entity, const FString& Name)
{
	// Create SQL query for delete row for the given entity and record.
	const FString Query = FString::Printf(TEXT("DELETE FROM %s WHERE Name = '%s';"), *Entity, *Name);
	// Execute the query.
	const auto Result = ExecuteDatabaseQuery(Query);
	// Check record count to see if the update was successful.
	if (Result != nullptr)
	{
		FB_VERBOSE("%s Deleted.", *Name);
	}
	delete Result;
}
