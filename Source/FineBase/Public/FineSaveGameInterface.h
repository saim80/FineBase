// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FineSaveGameInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFineSaveGameInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Provides a common interface for save game instances to be used by UFineSaveGameComponent.
 */
class FINEBASE_API IFineSaveGameInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category="FineBase")
	void ResetToDefaults();
};
