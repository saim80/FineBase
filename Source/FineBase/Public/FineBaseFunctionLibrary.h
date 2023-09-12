// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FineBaseFunctionLibrary.generated.h"

class FSQLiteResultSet;
class FSQLiteDatabaseConnection;
struct FFineDisplayData;
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

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static FString GetFilePathFromAssetPath(const FString& Directory, const FString& AssetPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static FString GetSaveGamePath(const FString& SaveGameName);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static FString GetDatabaseFileName(const FString& SlotName, const int32& UserIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static void DeletePlayerData(const FString& SlotName, const int32& UserIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	static FString GetProjectVersionString();
	
};
