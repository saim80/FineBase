// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "FineBaseLoggerMacros.h"

class FFineBaseModule : public IModuleInterface
{
public:
	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

// Declare Log Category
DECLARE_LOG_CATEGORY_EXTERN(LogFineBase, Log, All)

#define FB_FATAL(FORMAT, ...) FINEBASE_FATAL(Category, FORMAT, ##__VA_ARGS__)
#define FB_ERROR(FORMAT, ...) FINEBASE_ERROR(Category, FORMAT, ##__VA_ARGS__)
#define FB_WARNING(FORMAT, ...) FINEBASE_WARNING(Category, FORMAT, ##__VA_ARGS__)
#define FB_DISPLAY(FORMAT, ...) FINEBASE_DISPLAY(Category, FORMAT, ##__VA_ARGS__)
#define FB_LOG(VERBOSITY, FORMAT, ...) FINEBASE_LOG(Category, VERBOSITY, FORMAT, ##__VA_ARGS__)
#define FB_VERBOSE(FORMAT, ...) FINEBASE_VERBOSE(Category, FORMAT, ##__VA_ARGS__)
#define FB_VERYVERBOSE(FORMAT, ...) FINEBASE_VERYVERBOSE(Category, FORMAT, ##__VA_ARGS__)
