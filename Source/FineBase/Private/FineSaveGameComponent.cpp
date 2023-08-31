// (c) 2023 Pururum LLC. All rights reserved.


#include "FineSaveGameComponent.h"

#include "FineBase.h"
#include "FineSaveGameInterface.h"
#include "GameFramework/SaveGame.h"
#include "Kismet/GameplayStatics.h"


// Sets default values for this component's properties
UFineSaveGameComponent::UFineSaveGameComponent(): Super()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UFineSaveGameComponent::ResetProgress_Implementation()
{
	// Return if user index is less than zero.
	if (UserIndex < 0)
	{
		FB_WARNING("User index is less than zero.");
		return;
	}
	// Return if slot is empty.
	if (Slot.IsEmpty())
	{
		FB_WARNING("Slot is empty");
		return;
	}
	if (!IsValid(SaveGameClass))
	{
		FB_WARNING("No save game class set.")
		return;
	}
	OnSaveGameWillBeLoaded.Broadcast();
	// if save game exists on file, delete it.
	if (UGameplayStatics::DoesSaveGameExist(Slot, UserIndex))
	{
		UGameplayStatics::DeleteGameInSlot(Slot, UserIndex);
	}
	// create new save game by using the save game class.
	USaveGame* SaveGame = UGameplayStatics::CreateSaveGameObject(SaveGameClass);
	// if the new instance implements FineSaveGameInterface, call ResetProgress.
	if (SaveGame->Implements<UFineSaveGameInterface>())
	{
		IFineSaveGameInterface::Execute_ResetToDefaults(SaveGame);
	}
	// save the new save game instance.
	UGameplayStatics::SaveGameToSlot(SaveGame, Slot, UserIndex);
	OnSaveGameLoaded.Broadcast();
}

void UFineSaveGameComponent::LoadProgress()
{
	// Return if user index is less than zero.
	if (UserIndex < 0)
	{
		FB_WARNING("User index is less than zero.");
		return;
	}
	// Return if slot is empty.
	if (Slot.IsEmpty())
	{
		FB_WARNING("Slot is empty.");
		return;
	}
	// if save game doesn't exit on file, call ResetProgress then return.
	if (!UGameplayStatics::DoesSaveGameExist(Slot, UserIndex))
	{
		ResetProgress();
		return;
	}
	OnSaveGameWillBeLoaded.Broadcast();
	// synchronously load save game instance from file.
	USaveGame* SaveGame = UGameplayStatics::LoadGameFromSlot(Slot, UserIndex);
	check(IsValid(SaveGame));
	LoadedProgress = SaveGame;
	OnSaveGameLoaded.Broadcast();
}

void UFineSaveGameComponent::AsyncLoadProgress()
{
	// Return if user index is less than zero.
	if (UserIndex < 0)
	{
		FB_WARNING("User index is less than zero.");
		return;
	}
	// Return if slot is empty.
	if (Slot.IsEmpty())
	{
		FB_WARNING("Slot is empty.");
		return;
	}
	if (!IsValid(GetWorld()))
	{
		FB_WARNING("No world object found. Maybe game didn't start yet.")
		return;
	}
	// Clear the load timer handle.
	GetWorld()->GetTimerManager().ClearTimer(LoadTimer);
	const auto Self = TWeakObjectPtr<UFineSaveGameComponent>(this);
	// Set the load timer for non-repeating lambda function for 0.2 seconds.
	GetWorld()->GetTimerManager().SetTimer(LoadTimer, [=]()
	{
		if (!Self.IsValid()) return;
		// if save game doesn't exit on file, call ResetProgress then return.
		if (!UGameplayStatics::DoesSaveGameExist(Self->Slot, Self->UserIndex))
		{
			Self->ResetProgress();
			return;
		}
		Self->OnSaveGameWillBeLoaded.Broadcast();
		// asynchronously load save game instance from file.
		UGameplayStatics::AsyncLoadGameFromSlot(
			Self->Slot, Self->UserIndex,
			FAsyncLoadGameFromSlotDelegate::CreateLambda(
				[=](auto _Slot, auto _Index, auto _SaveGame)
				{
					// if weak self is valid, call OnSaveGameLoaded.
					if (!Self.IsValid()) return;
					Self->LoadedProgress = _SaveGame;
					Self->OnSaveGameLoaded.Broadcast();
				}));
	}, 0.2f, false);
}

void UFineSaveGameComponent::SaveProgress()
{
	// Return if user index is less than zero.
	if (UserIndex < 0)
	{
		FB_WARNING("User index is less than zero.");
		return;
	}
	// Return if slot is empty.
	if (Slot.IsEmpty())
	{
		FB_WARNING("Slot is empty.");
		return;
	}
	// save game instance if it's valid.
	if (IsValid(LoadedProgress))
	{
		OnSaveGameWillBeSaved.Broadcast();
		const auto Result = UGameplayStatics::SaveGameToSlot(LoadedProgress, Slot, UserIndex);
		OnSaveGameSaved.Broadcast(Slot, UserIndex, Result);
	}
}

void UFineSaveGameComponent::AsyncSaveProgress()
{
	// Return if user index is less than zero.
	if (UserIndex < 0)
	{
		FB_WARNING("User index is less than zero.");
		return;
	}
	// Return if slot is empty.
	if (Slot.IsEmpty())
	{
		FB_WARNING("Slot is empty.");
		return;
	}
	// Clear the save timer handle.
	GetWorld()->GetTimerManager().ClearTimer(SaveTimer);
	const auto Self = TWeakObjectPtr<UFineSaveGameComponent>(this);
	// Set the save timer for non-repeating lambda function for 0.2 seconds.
	GetWorld()->GetTimerManager().SetTimer(SaveTimer, [=]()
	{
		// if Self is invalid, return
		if (!Self.IsValid()) return;
		// asynchronously save game instance if it's valid.
		if (ensure(IsValid(Self->LoadedProgress)))
		{
			Self->OnSaveGameWillBeSaved.Broadcast();
			UGameplayStatics::AsyncSaveGameToSlot(
				Self->LoadedProgress, Self->Slot, Self->UserIndex,
				FAsyncSaveGameToSlotDelegate::CreateLambda(
					[=](auto _Slot, auto _Index, auto Success)
					{
						// if weak self is valid, call OnSaveGameSaved.
						if (!Self.IsValid()) return;
						Self->OnSaveGameSaved.Broadcast(Self->Slot, Self->UserIndex, Success);
					}));
		}
	}, 0.2f, false);
}

void UFineSaveGameComponent::SetUserIndex(int32 Index)
{
	if (Index != UserIndex)
	{
		UserIndex = Index;
		AsyncLoadProgress();
	}
}

void UFineSaveGameComponent::SetSlot(FString NewSlot)
{
	// if new slot is different from the current slot, assign new slot to the current slot.
	if (NewSlot != Slot)
	{
		Slot = NewSlot;
		AsyncLoadProgress();
	}
}

bool UFineSaveGameComponent::IsLoaded()
{
	return IsValid(LoadedProgress);
}

void UFineSaveGameComponent::BeginPlay()
{
	Super::BeginPlay();
	AsyncLoadProgress();
}

void UFineSaveGameComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
	LoadedProgress = nullptr;
}
