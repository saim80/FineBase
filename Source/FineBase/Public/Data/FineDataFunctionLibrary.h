// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FineDataFunctionLibrary.generated.h"

class UFineLocalDatabaseComponent;
struct FFineDisplayData;
struct FFineActorData;
struct FFineCharacterData;
/**
 * 
 */
UCLASS()
class FINEBASE_API UFineDataFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FFineCharacterData GetCharacterData(const FString& CharacterName, UFineLocalDatabaseComponent* LocalDatabase);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FFineActorData GetActorData(const FString& ActorName, UFineLocalDatabaseComponent* LocalDatabase);
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static FFineDisplayData GetDisplayData(const FString& DisplayDataName, UFineLocalDatabaseComponent* LocalDatabase);
};
