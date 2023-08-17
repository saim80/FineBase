#pragma once

#include "CoreMinimal.h"
#include "FineBase.h"

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

#define FINEBASE_PRINT(Category, Verbosity, Format, ...) UE_LOG(Category, Verbosity, TEXT("<%s(%i)> %s"), *FString(__PRETTY_FUNCTION__), __LINE__, *FString::Printf(TEXT(Format), ##__VA_ARGS__))

#if (UE_BUILD_DEBUG || UE_BUILD_DEVELOPMENT)

#define FINEBASE_FATAL(Category, Format, ...) FINEBASE_PRINT(Category, Fatal, Format, ##__VA_ARGS__)
#define FINEBASE_ERROR(Category, Format, ...) FINEBASE_PRINT(Category, Error, Format, ##__VA_ARGS__)
#define FINEBASE_WARNING(Category, Format, ...) FINEBASE_PRINT(Category, Warning, Format, ##__VA_ARGS__)
#define FINEBASE_DISPLAY(Category, Format, ...) FINEBASE_PRINT(Category, Display, Format, ##__VA_ARGS__)
#define FINEBASE_LOG(Category, Format, ...) FINEBASE_PRINT(Category, Log, Format, ##__VA_ARGS__)
#define FINEBASE_VERBOSE(Category, Format, ...) FINEBASE_PRINT(Category, Verbose, Format, ##__VA_ARGS__)
#define FINEBASE_VERYVERBOSE(Category, Format, ...) FINEBASE_PRINT(Category, VeryVerbose, Format, ##__VA_ARGS__)

#else

#define FINEBASE_FATAL(Category, Format, ...)
#define FINEBASE_ERROR(Category, Format, ...)
#define FINEBASE_WARNING(Category, Format, ...)
#define FINEBASE_DISPLAY(Category, Format, ...)
#define FINEBASE_LOG(Category, Format, ...)
#define FINEBASE_VERBOSE(Category, Format, ...)
#define FINEBASE_VERYVERBOSE(Category, Format, ...)

#endif
