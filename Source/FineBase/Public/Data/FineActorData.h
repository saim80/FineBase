// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineDisplayData.h"
#include "FineActorData.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FINEBASE_API FFineActorData : public FFineDisplayData
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineActorInfo")
	int32 Health = 1;

	virtual void UpdateFromRecord(const FFineDatabaseRecord& Record) override;
	virtual FFineDatabaseRecord ToRecord() const override;
};
