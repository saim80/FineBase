// (c) 2023 Pururum LLC. All rights reserved.


#include "FineCountedFlag.h"

#include "FineBase.h"

void UFineCountedFlag::OnFlagValueUpdated_Implementation()
{
	FB_VERBOSE("Flag {0} updated to {1}", *Name.ToString(), IsEnabled());
	OnFlagUpdated.Broadcast(this, IsEnabled());
}

void UFineCountedFlag::SetEnabled(const bool NewFlag)
{
	// if the new flag is different from the current flag
	if (NewFlag != IsEnabled())
	{
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
		// call the event
		OnFlagValueUpdated();
	}
}
