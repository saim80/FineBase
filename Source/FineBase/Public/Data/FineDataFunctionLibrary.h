﻿// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FineDataFunctionLibrary.generated.h"

class UFineLocalDatabaseComponent;
struct FFineDisplayData;
/**
 * 
 */
UCLASS()
class FINEBASE_API UFineDataFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FFineDisplayData GetDisplayData(const FString& DisplayDataName, UFineLocalDatabaseComponent* LocalDatabase);
};
