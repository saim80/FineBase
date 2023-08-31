// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineLocalDatabaseComponent.h"

#include "FineBase.h"
#include "FineBaseFunctionLibrary.h"
#include "SQLiteDatabaseConnection.h"
#include "Data/FineDatabaseRecord.h"

UFineLocalDatabaseComponent::UFineLocalDatabaseComponent(const FObjectInitializer& ObjectInitializer): Super(
	ObjectInitializer)
{
	DatabasePath = TEXT("");
}

FString UFineLocalDatabaseComponent::GetDatabaseFilePath() const
{
	return UFineBaseFunctionLibrary::GetFilePathFromAssetPath(FPaths::ProjectContentDir(), DatabasePath);
}


void UFineLocalDatabaseComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	CloseConnection();
	Super::EndPlay(EndPlayReason);
}

bool UFineLocalDatabaseComponent::OpenConnection()
{
	const auto FilePath = GetDatabaseFilePath();
	FB_LOG("Opening database at %s", *FilePath);

	CloseConnection();
	if (DatabasePath.IsEmpty())
	{
		FB_WARNING("Database asset path is empty.");
		return false;
	}

	Connection = MakeShared<FSQLiteDatabaseConnection>();

	if (!Connection->Open(*FilePath, TEXT(""), TEXT("")))
	{
		FB_ERROR("Failed to open database.");
		Connection = nullptr;
		return false;
	}
	FB_LOG("Database opened successfully.");
	OnDatabaseConnected.Broadcast();
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
	if (ResultSet->GetRecordCount() > 0)
	{
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
	}
#endif

	bSuccess = true;
	TArray<FFineDatabaseRecord> Records;
	if (ResultSet->GetRecordCount() > 0)
	{
		// Iterate the result set and make FFineDatabaseRecord for each row.
		for (FSQLiteResultSet::TIterator It(ResultSet); It; ++It)
		{
			const auto Row = *It;
			Records.Add(FFineDatabaseRecord::FromResultSet(Row));
		}
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
