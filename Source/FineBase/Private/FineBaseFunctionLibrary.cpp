// (c) 2023 Pururum LLC. All rights reserved.


#include "FineBaseFunctionLibrary.h"


bool UFineBaseFunctionLibrary::CanQuitGame()
{
	// if the platform is mobile, console, or VR, return false.
	return PLATFORM_DESKTOP;
}

double UFineBaseFunctionLibrary::GetTimeSecondsSince1970(const FDateTime& DateTime)
{
	// Return the time in seconds since 1970 for a given date time.
	return FTimespan(DateTime.ToUnixTimestamp()).GetTotalSeconds();
}
