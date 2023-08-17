// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineDisplayData.generated.h"

struct FFineDatabaseRecord;
/**
 * Display information.
 */
USTRUCT(BlueprintType)
struct FINEBASE_API FFineDisplayData
{
	GENERATED_BODY()
	
	FFineDisplayData() = default;
	virtual ~FFineDisplayData() = default;
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDisplayInfo")
	FText SingularTitle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDisplayInfo")
	FText PluralTitle;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDisplayInfo", meta = (MultiLine = true))
	FText Description;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDisplayInfo")
	FName Name;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "FineDisplayInfo")
	TSoftObjectPtr<UTexture2D> Image;

	virtual void UpdateFromRecord(const FFineDatabaseRecord& Record);
	virtual FFineDatabaseRecord ToRecord() const;
	static FFineDisplayData FromRecord(const FFineDatabaseRecord& Record);
};
