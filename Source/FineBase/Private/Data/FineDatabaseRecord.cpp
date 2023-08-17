// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineDatabaseRecord.h"

#include "FineBase.h"
#include "SQLiteResultSet.h"

FFineDatabaseRecord FFineDatabaseRecord::FromResultSet(const FDataBaseRecordSet* ResultSet)
{
	FFineDatabaseRecord OutRecord;
	OutRecord.UpdateFromResultSet(ResultSet);
	return OutRecord;
}

void FFineDatabaseRecord::UpdateFromResultSet(const FDataBaseRecordSet* ResultSet)
{
	const auto  AllColumns = ResultSet->GetColumnNames();
	// Iterate over all the fields in the result set
	for (const auto Field : AllColumns)
	{
		// Get the field name
		auto FieldName = Field.ColumnName;

		// Get the field type
		const auto FieldType = Field.DataType;

		// for each field type, add the field to the appropriate map
		if (FieldType == DBT_INT)
		{
			IntFields.Add(FieldName, ResultSet->GetInt(*FieldName));
		}
		else if (FieldType == DBT_FLOAT)
		{
			FloatFields.Add(FieldName, ResultSet->GetFloat(*FieldName));
		}
		else if (FieldType == DBT_STRING)
		{
			StringFields.Add(FieldName, ResultSet->GetString(*FieldName));
		}
		else if (FieldType == DBT_UNKOWN)
		{
			FB_ERROR("Unknown field type for field %s", *FieldName);
		}
	}
}
