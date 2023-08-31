// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineUserDatabaseComponent.h"

#include "FineBase.h"
#include "FineBaseFunctionLibrary.h"
#include "Data/FineDatabaseRecord.h"

UFineUserDatabaseComponent::UFineUserDatabaseComponent(const FObjectInitializer& ObjectInitializer): Super(
	ObjectInitializer)
{
	TemplatePath = TEXT("");
}

void UFineUserDatabaseComponent::ResetDatabase()
{
	// if connection is open, print warning and return.
	if (Connection.IsValid())
	{
		FB_WARNING("Database is already opened. Close the connection before resetting.");
		return;
	}
	if (GetDatabaseAssetPath().IsEmpty())
	{
		FB_WARNING("Database asset path is empty.");
		return;
	}
	// if the database file exists at the path, delete it.
	const auto DatabaseFilePath = GetDatabaseFilePath();
	if (FPaths::FileExists(DatabaseFilePath))
	{
		FB_LOG("Deleting database at %s", *DatabaseFilePath);
		FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*DatabaseFilePath);
	}
	// if template file exists at the path, copy it to the database file path.
	const auto TemplateFilePath = GetTemplateFilePath();
	if (FPaths::FileExists(TemplateFilePath))
	{
		FB_LOG("Copying template database from %s to %s", *TemplateFilePath, *DatabaseFilePath);
		FPlatformFileManager::Get().GetPlatformFile().CopyFile(*DatabaseFilePath, *TemplateFilePath);
	}
	else
	{
		FB_WARNING("Template database file does not exist at %s", *TemplateFilePath);
		// create an empty file at the database file path.
		FFileHelper::SaveStringToFile(TEXT(""), *DatabaseFilePath);
	}
}

FString UFineUserDatabaseComponent::GetDatabaseFilePath() const
{
	return UFineBaseFunctionLibrary::GetSaveGamePath(GetDatabaseAssetPath());
}

bool UFineUserDatabaseComponent::OpenConnection()
{
	if (!FPaths::FileExists(GetDatabaseFilePath()))
	{
		FB_WARNING("Database file does not exist at %s", *GetDatabaseFilePath());
		ResetDatabase();
	}
	return Super::OpenConnection();
}

FString UFineUserDatabaseComponent::GetTemplateFilePath() const
{
	const auto FilePath = UFineBaseFunctionLibrary::GetFilePathFromAssetPath(
		FPaths::ProjectContentDir(), TemplatePath);
	return FilePath;
}

bool UFineUserDatabaseComponent::UpdateRecordByName(const FString& Entity, const FName& Name,
                                                    const FFineDatabaseRecord& Record)
{
	return UpdateRecords(Entity, FString::Printf(TEXT("Name = '%s'"), *Name.ToString()), Record);
}

bool UFineUserDatabaseComponent::UpdateRecords(const FString& Entity, const FString& WhereClause,
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
	Query += FString::Printf(TEXT(" WHERE %s"), *WhereClause);
	// Add semicolon to end the statement.
	Query += TEXT(";");
	bool bSuccess;
	// Execute the query.
	const auto Result = ExecuteQuery(Query, bSuccess);
	// Check record count to see if the update was successful.
	if (bSuccess)
	{
		FB_VERBOSE("%s Updated.", *WhereClause);
	}
	return bSuccess;
}

bool UFineUserDatabaseComponent::CreateRecord(const FString& Entity, const FName& Name,
                                              const FFineDatabaseRecord& Record)
{
	// Create SQL query for insert row for the given entity and record.
	FString Query = FString::Printf(TEXT("INSERT INTO %s ("), *Entity);

	if (Name != NAME_None)
	{
		// Add 'Name" as field.
		Query += TEXT("Name, ");
	}

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
	if (Name != NAME_None)
	{
		// Add the name value.
		Query += FString::Printf(TEXT("'%s', "), *Name.ToString());
	}
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
		FB_VERBOSE("%s Created.", *Name.ToString());
	}

	return bSuccess;
}

bool UFineUserDatabaseComponent::DeleteRecordByName(const FString& Entity, const FString& Name)
{
	// Create where clause
	const FString WhereClause = FString::Printf(TEXT("Name = '%s'"), *Name);
	return DeleteRecords(Entity, WhereClause);
}

bool UFineUserDatabaseComponent::DeleteRecords(const FString& Entity, const FString& WhereClause)
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
