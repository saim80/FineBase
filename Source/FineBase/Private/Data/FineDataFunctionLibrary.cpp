// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineDataFunctionLibrary.h"

#include "Data/FineDatabaseRecord.h"
#include "Data/FineDisplayData.h"
#include "Data/FineLocalDatabaseComponent.h"


FFineDisplayData UFineDataFunctionLibrary::GetDisplayData(const FString& DisplayDataName,
	UFineLocalDatabaseComponent* LocalDatabase)
{
	bool bSuccess;
	const auto Record = LocalDatabase->GetRecordByName(TEXT("DisplayData"), *DisplayDataName, bSuccess);
	return FFineDisplayData::FromRecord(Record);
}
