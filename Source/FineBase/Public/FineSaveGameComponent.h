// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FineSaveGameComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameWillBeLoaded);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameLoaded);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnSaveGameWillBeSaved);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnSaveGameSaved, const FString&, Slot, const int32, Index, bool,
                                               bSuccess);

class USaveGame;
/// This class is responsible for managing save games for their loading and saving.
UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class FINEBASE_API UFineSaveGameComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UFineSaveGameComponent();

	UPROPERTY(BlueprintAssignable)
	FOnSaveGameWillBeLoaded OnSaveGameWillBeLoaded;
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameLoaded OnSaveGameLoaded;
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameWillBeSaved OnSaveGameWillBeSaved;
	UPROPERTY(BlueprintAssignable)
	FOnSaveGameSaved OnSaveGameSaved;

	UFUNCTION(BlueprintCallable, CallInEditor, Category="FineBase")
	void ResetProgress();
	UFUNCTION(BlueprintCallable, Category="FineBase")
	void LoadProgress();
	UFUNCTION(BlueprintCallable, Category="FineBase")
	void AsyncLoadProgress();
	UFUNCTION(BlueprintCallable, Category="FineBase")
	void SaveProgress();
	UFUNCTION(BlueprintCallable, Category="FineBase")
	void AsyncSaveProgress();

	UFUNCTION(BlueprintCallable, Category="FineBase")
	void SetUserIndex(int32 Index);
	UFUNCTION(BlueprintCallable, Category="FineBase")
	void SetSlot(FString NewSlot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="FineBase")
	bool IsLoaded();

	FORCEINLINE const int32& GetUserIndex() const { return UserIndex; }

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UPROPERTY(BlueprintReadOnly, Category="FineBase", meta=(AllowPrivateAccess="true"))
	USaveGame* LoadedProgress;

private:
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="FineBase", meta=(AllowPrivateAccess="true"))
	FString Slot;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="FineBase", meta=(AllowPrivateAccess="true"))
	int32 UserIndex = -1;
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category="FineBase", meta=(AllowPrivateAccess="true"))
	TSubclassOf<USaveGame> SaveGameClass;

	FTimerHandle LoadTimer;
	FTimerHandle SaveTimer;
};
