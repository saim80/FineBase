// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineActorData.h"
#include "FineCharacterData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FINEBASE_API FFineCharacterData : public FFineActorData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineCharacterInfo")
	int32 Mana = 0;

	virtual void UpdateFromRecord(const FFineDatabaseRecord& Record) override;
	virtual FFineDatabaseRecord ToRecord() const override;
	static FFineCharacterData FromRecord(const FFineDatabaseRecord& Record);
};
