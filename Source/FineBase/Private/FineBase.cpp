// Copyright Epic Games, Inc. All Rights Reserved.

#include "FineBase.h"

#define LOCTEXT_NAMESPACE "FFineBaseModule"

void FFineBaseModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FFineBaseModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FFineBaseModule, FineBase)
DEFINE_LOG_CATEGORY(LogFineBase)