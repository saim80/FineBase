// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineCharacterData.h"

#include "Data/FineDatabaseRecord.h"

void FFineCharacterData::UpdateFromRecord(const FFineDatabaseRecord& Record)
{
	Super::UpdateFromRecord(Record);
	
	Mana = Record.IntFields[TEXT("Mana")];
	AttackRating = Record.IntFields[TEXT("AttackRating")];
}

FFineDatabaseRecord FFineCharacterData::ToRecord() const
{
	auto Record = Super::ToRecord();

	Record.IntFields[TEXT("Mana")] = Mana;
	Record.IntFields[TEXT("AttackRating")] = AttackRating;

	return Record;
}
