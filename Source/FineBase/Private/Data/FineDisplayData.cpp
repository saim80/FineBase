// (c) 2023 Pururum LLC. All rights reserved.


#include "Data/FineDisplayData.h"

#include "Data/FineDatabaseRecord.h"

void FFineDisplayData::UpdateFromRecord(const FFineDatabaseRecord& Record)
{
	Name = FName(*Record.StringFields[TEXT("Name")]);
	SingularTitle = FText::FromString(Record.StringFields[TEXT("SingularTitle")]);
	PluralTitle = FText::FromString(Record.StringFields[TEXT("PluralTitle")]);
	Description = FText::FromString(Record.StringFields[TEXT("Description")]);
	Image = TSoftObjectPtr<UTexture2D>(FSoftObjectPath(Record.StringFields[TEXT("Image")]));
}

FFineDatabaseRecord FFineDisplayData::ToRecord() const
{
	FFineDatabaseRecord Record;

	Record.StringFields[TEXT("SingularTitle")] = SingularTitle.ToString();
	Record.StringFields[TEXT("PluralTitle")] = PluralTitle.ToString();
	Record.StringFields[TEXT("Description")] = Description.ToString();
	Record.StringFields[TEXT("Image")] = Image.ToSoftObjectPath().ToString();

	return Record;
}
