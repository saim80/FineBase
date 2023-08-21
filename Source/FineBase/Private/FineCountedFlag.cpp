// (c) 2023 Pururum LLC. All rights reserved.


#include "FineCountedFlag.h"

#include "FineBase.h"

void UFineCountedFlag::OnFlagValueUpdated_Implementation()
{
	FB_LOG("Flag %s updated to %s", *Name.ToString(), IsEnabled() ? TEXT("enabled") : TEXT("disabled"));
	OnFlagUpdated.Broadcast(this, IsEnabled());
}

void UFineCountedFlag::SetEnabled(const bool NewFlag)
{
	// if the new flag is different from the current flag
	const auto OldCounter = Counter;
	// if the new flag is on
	if (NewFlag)
	{
		// increment the counter
		Counter++;
	}
	else
	{
		// decrement the counter
		Counter--;
	}
	if (OldCounter <= 0 && Counter > 0 || OldCounter > 0 && Counter <= 0)
	{
		// call the event
		OnFlagValueUpdated();
	}
}
