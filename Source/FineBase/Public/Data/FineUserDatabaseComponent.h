// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "FineLocalDatabaseComponent.h"
#include "UObject/Object.h"
#include "FineUserDatabaseComponent.generated.h"

/**
 * Writable database. This is used to store user data.
 */
UCLASS()
class FINEBASE_API UFineUserDatabaseComponent : public UFineLocalDatabaseComponent
{
	GENERATED_BODY()

public:
	UFineUserDatabaseComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, CallInEditor, Category="FineBase")
	void ResetDatabase();

	virtual FString GetDatabaseFilePath() const override;

	UFUNCTION(BlueprintCallable)
	bool UpdateRecordByName(const FString& Entity, const FName& Name, const FFineDatabaseRecord& Record);
	UFUNCTION(BlueprintCallable)
	bool UpdateRecords(const FString& Entity, const FString& WhereClause, const FFineDatabaseRecord& Record);
	UFUNCTION(BlueprintCallable)
	bool CreateRecord(const FString& Entity, const FName& Name, const FFineDatabaseRecord& Record);
	UFUNCTION(BlueprintCallable)
	bool DeleteRecordByName(const FString& Entity, const FString& Name);
	UFUNCTION(BlueprintCallable)
	bool DeleteRecords(const FString& Entity, const FString& WhereClause);
	
	virtual bool OpenConnection() override;

private:
	/// The template database to copy from.
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "FineBase", meta = (AllowPrivateAccess = "true"))
	FString TemplatePath;

	FString GetTemplateFilePath() const;
};
