// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineLocalDatabaseComponent.h"

#include "FineBase.h"
#include "SQLiteDatabaseConnection.h"
#include "Data/FineDatabaseRecord.h"

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

void UFineLocalDatabaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseConnection();
	Super::EndPlay(EndPlayReason);
}

bool UFineLocalDatabaseComponent::OpenConnection()
{
	const auto FilePath = GetDatabaseConnectionString();
	FB_LOG("Opening database at %s", *FilePath);

	CloseConnection();

	Connection = MakeShared<FSQLiteDatabaseConnection>();

	if (!Connection->Open(*FilePath, TEXT(""), TEXT("")))
	{
		FB_ERROR("Failed to open database.");
		return false;
	}
	FB_LOG("Database opened successfully.");
	return true;
}

void UFineLocalDatabaseComponent::CloseConnection()
{
	if (Connection.IsValid())
	{
		Connection->Close();
		Connection = nullptr;
	}
}

TArray<FFineDatabaseRecord> UFineLocalDatabaseComponent::ExecuteQuery(const FString& Query, bool& bSuccess)
{
	// Open the connection if it is not opened.
	if (!Connection.IsValid())
	{
		if (!OpenConnection())
		{
			bSuccess = false;
			return {};
		}
	}
	// Execute the query and get the result set.
	FSQLiteResultSet* ResultSet = nullptr;
	if (!Connection->Execute(*Query, ResultSet))
	{
		if (!Connection->GetLastError().IsEmpty())
		{
			FB_ERROR("SQLite query failed: %s %s", *Query, *Connection->GetLastError());
		}
		else
		{
			FB_ERROR("SQLite query failed: %s", *Query);
		}
		bSuccess = false;
		return {};
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

	bSuccess = true;
	// Iterate the result set and make FFineDatabaseRecord for each row.
	TArray<FFineDatabaseRecord> Records;
	for (FSQLiteResultSet::TIterator It(ResultSet); It; ++It)
	{
		const auto Row = *It;
		Records.Add(FFineDatabaseRecord::FromResultSet(Row));
	}
	delete ResultSet;
	return Records;
}

FFineDatabaseRecord UFineLocalDatabaseComponent::GetRecordByName(const FString Entity, const FName Name,
                                                                 bool& bSuccess)
{
	const FString Query = FString::Printf(TEXT("SELECT * FROM %s WHERE Name = '%s';"), *Entity, *Name.ToString());
	const auto Records = ExecuteQuery(Query, bSuccess);
	if (Records.IsEmpty())
	{
		bSuccess = false;
		return {};
	}
	return Records[0];
}

TArray<FFineDatabaseRecord> UFineLocalDatabaseComponent::GetRecords(const FString& Entity, bool& bSuccess)
{
	// Build the query.
	const FString Query = FString::Printf(TEXT("SELECT * FROM %s;"), *Entity);
	// Execute the query and get the result set.
	return ExecuteQuery(Query, bSuccess);
}

TArray<FFineDatabaseRecord> UFineLocalDatabaseComponent::FilterRecords(const FString& Entity,
                                                                       const FString& WhereClause, bool& bSuccess)
{
	// Build the query.
	const FString Query = FString::Printf(TEXT("SELECT * FROM %s WHERE %s;"), *Entity, *WhereClause);
	// Execute the query and get the result set.
	return ExecuteQuery(Query, bSuccess);
}

bool UFineLocalDatabaseComponent::UpdateRecordByName(const FString& Entity, const FString& Name,
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
	bool bSuccess;
	// Execute the query.
	const auto Result = ExecuteQuery(Query, bSuccess);
	// Check record count to see if the update was successful.
	if (bSuccess)
	{
		FB_VERBOSE("%s Updated.", *Name);
	}
	return bSuccess;
}

bool UFineLocalDatabaseComponent::CreateRecord(const FString& Entity, const FString& Name,
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

	bool bSuccess;
	// Execute the query.
	const auto Result = ExecuteQuery(Query, bSuccess);
	// Check record count to see if the update was successful.
	if (bSuccess)
	{
		FB_VERBOSE("%s Created.", *Name);
	}

	return bSuccess;
}

bool UFineLocalDatabaseComponent::DeleteRecordByName(const FString& Entity, const FString& Name)
{
	// Create where clause
	const FString WhereClause = FString::Printf(TEXT("Name = '%s'"), *Name);
	return DeleteRecords(Entity, WhereClause);
}

bool UFineLocalDatabaseComponent::DeleteRecords(const FString& Entity, const FString& WhereClause)
{
	// Create SQL query for delete row for the given entity and record.
	const FString Query = FString::Printf(TEXT("DELETE FROM %s WHERE %s;"), *Entity, *WhereClause);
	bool bSuccess;
	// Execute the query.
	const auto Result = ExecuteQuery(Query, bSuccess);
	// Check record count to see if the update was successful.
	if (bSuccess)
	{
		FB_VERBOSE("Records Deleted.");
	}
	return bSuccess;
}
