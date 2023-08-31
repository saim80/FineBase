// (c) 2023 Pururum LLC. All rights reserved.


#include "FineBaseFunctionLibrary.h"

#include "Kismet/GameplayStatics.h"


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

FString UFineBaseFunctionLibrary::GetFilePathFromAssetPath(const FString& Directory, const FString& AssetPath)
{
	// Get the game directory.
	const FString GameDir = FPaths::ConvertRelativePathToFull(Directory);
	// Check if the path has a prefix of /Game, then remove it.
	FString StrippedPath = AssetPath.StartsWith("/Game") ? AssetPath.RightChop(5) : AssetPath;
	// Check if the path has a prefix of /, then remove it.
	StrippedPath = StrippedPath.StartsWith("/") ? StrippedPath.RightChop(1) : StrippedPath;

	// Get the file path by using path class.
	const FString FilePath = FPaths::Combine(GameDir, StrippedPath);

	return FilePath;
}

FString UFineBaseFunctionLibrary::GetSaveGamePath(const FString& SaveGameName)
{
	const auto SaveDirectory = FPaths::Combine(FPaths::ProjectSavedDir(), TEXT("SaveGames"));
	const auto FilePath = UFineBaseFunctionLibrary::GetFilePathFromAssetPath(SaveDirectory, SaveGameName);
	return FilePath;
}

FString UFineBaseFunctionLibrary::GetDatabaseFileName(const FString& SlotName, const int32& UserIndex)
{
	return FString::Printf(TEXT("%s-%d.db"), *SlotName, UserIndex);
}

void UFineBaseFunctionLibrary::DeletePlayerData(const FString& SlotName, const int32& UserIndex)
{
	if (UGameplayStatics::DoesSaveGameExist(SlotName, UserIndex))
	{
		UGameplayStatics::DeleteGameInSlot(SlotName, UserIndex);
	}
	const auto DatabasePath = GetSaveGamePath(GetDatabaseFileName(SlotName, UserIndex));
	if (FPaths::FileExists(DatabasePath))
	{
		auto& File = FPlatformFileManager::Get().GetPlatformFile();
		File.DeleteFile(*DatabasePath);
	}
}
