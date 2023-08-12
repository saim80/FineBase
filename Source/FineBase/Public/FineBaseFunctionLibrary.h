// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FineBaseFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class FINEBASE_API UFineBaseFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static bool CanQuitGame();

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static double GetTimeSecondsSince1970(const FDateTime& DateTime);
};
