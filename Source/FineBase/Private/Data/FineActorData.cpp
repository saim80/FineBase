// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineActorData.h"

#include "Data/FineDatabaseRecord.h"


void FFineActorData::UpdateFromRecord(const FFineDatabaseRecord& Record)
{
	Super::UpdateFromRecord(Record);

	Health = Record.IntFields[TEXT("Health")];
	DefenseRating = Record.IntFields[TEXT("DefenseRating")];
}

FFineDatabaseRecord FFineActorData::ToRecord() const
{
	auto Record = Super::ToRecord();

	Record.IntFields[TEXT("Health")] = Health;
	Record.IntFields[TEXT("DefenseRating")] = DefenseRating;

	return Record;
}
