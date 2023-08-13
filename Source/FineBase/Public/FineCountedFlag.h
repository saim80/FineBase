// (c) 2023 Pururum LLC. All rights reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "FineCountedFlag.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnFlagUpdated, UFineCountedFlag*, Flag, const bool, NewFlag);

/**
 * A flag that controlled by counter. Counter <= 0 means the flag is off. Counter > 0 means the flag is on.
 */
UCLASS(BlueprintType)
class FINEBASE_API UFineCountedFlag : public UActorComponent
{
	GENERATED_BODY()

public:
	/// Sets the flag value. If the new flag is on, counter is incremented.
	/// If the new flag is off, counter is decremented.
	UFUNCTION(BlueprintCallable)
	void SetEnabled(const bool& NewFlag);

	/// Determines if the flag is on.
	UFUNCTION(BlueprintCallable, BlueprintPure)
	bool IsEnabled() const { return Counter > 0; }

	/// Event to expose the flag value change.
	UPROPERTY(BlueprintAssignable)
	FOnFlagUpdated OnFlagUpdated;

protected:
	/// Called when the flag value is updated.
	UFUNCTION(BlueprintNativeEvent, meta = (AllowPrivateAccess = true))
	void OnFlagValueUpdated();
	void OnFlagValueUpdated_Implementation();

private:
	int32 Counter;

	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "Flag", meta = (AllowPrivateAccess = true))
	FName Name;
};
