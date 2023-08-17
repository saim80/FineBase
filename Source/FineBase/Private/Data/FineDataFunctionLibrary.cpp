// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineDataFunctionLibrary.h"

#include "Data/FineActorData.h"
#include "Data/FineDatabaseRecord.h"
#include "Data/FineDisplayData.h"
#include "Data/FineLocalDatabaseComponent.h"


FFineActorData UFineDataFunctionLibrary::GetActorData(const FString& ActorName,
	UFineLocalDatabaseComponent* LocalDatabase)
{
	bool bSuccess;
	const auto Record = LocalDatabase->GetRecordByName(TEXT("Actor"), *ActorName, bSuccess);
	return FFineActorData::FromRecord(Record);
}

FFineDisplayData UFineDataFunctionLibrary::GetDisplayData(const FString& DisplayDataName,
	UFineLocalDatabaseComponent* LocalDatabase)
{
	bool bSuccess;
	const auto Record = LocalDatabase->GetRecordByName(TEXT("DisplayData"), *DisplayDataName, bSuccess);
	return FFineDisplayData::FromRecord(Record);
}
