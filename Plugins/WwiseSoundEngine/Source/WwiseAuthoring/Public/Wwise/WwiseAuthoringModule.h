/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2024 Audiokinetic Inc.
*******************************************************************************/

#pragma once

#include "Misc/CommandLine.h"
#include "Misc/ConfigCacheIni.h"
#include "Modules/ModuleManager.h"

class IWAAPI;

class IWwiseAuthoringModule : public IModuleInterface
{
public:
	static WWISEAUTHORING_API IWAAPI* WAAPI;

	static FName GetModuleName()
	{
		static const FName ModuleName = GetModuleNameFromConfig();
		return ModuleName;
	}

	/**
	 * Checks to see if this module is loaded and ready.
	 *
	 * @return True if the module is loaded and ready to use
	 */
	static bool IsAvailable()
	{
		static bool bModuleAvailable = false;
		if (LIKELY(!IsEngineExitRequested()) && LIKELY(bModuleAvailable))
		{
			return true;
		}
		bModuleAvailable = FModuleManager::Get().IsModuleLoaded(GetModuleName());
		return bModuleAvailable;
	}

	static void ForceLoadModule()
	{
		static bool bModuleLoadProcessed = false;
		if (LIKELY(bModuleLoadProcessed))
		{
			return;
		}

		const auto ModuleName = GetModuleName();
		if (ModuleName.IsNone())
		{
			bModuleLoadProcessed = true;
			return;
		}

		if (UNLIKELY(IsEngineExitRequested()))
		{
			UE_LOG(LogLoad, Log, TEXT("Skipping reloading missing WwiseAuthoring module: Exiting."));
		}
		else if (UNLIKELY(!IsInGameThread()))
		{
			UE_LOG(LogLoad, Warning, TEXT("Skipping loading missing WwiseAuthoring module: Not in game thread"));
		}
		else
		{
			FModuleManager& ModuleManager = FModuleManager::Get();
			UE_LOG(LogLoad, Log, TEXT("Loading WAAPI module: %s"), *ModuleName.GetPlainNameString());
			const auto* Module = ModuleManager.LoadModulePtr<IWwiseAuthoringModule>(ModuleName);
			bModuleLoadProcessed = true;
			if (UNLIKELY(!Module))
			{
				UE_LOG(LogLoad, Fatal, TEXT("Could not load WwiseAuthoring module: %s not found"), *ModuleName.GetPlainNameString());
			}
		}
	}

private:
	static inline FName GetModuleNameFromConfig()
	{
		FString ModuleName = TEXT("WwiseAuthoring");
		GConfig->GetString(TEXT("Audio"), TEXT("WwiseAuthoringModuleName"), ModuleName, GEngineIni);
		return FName(ModuleName);
	}
};

class WWISEAUTHORING_API FWwiseAuthoringModule : public IWwiseAuthoringModule
{
public:
	void StartupModule() override;
	void ShutdownModule() override;
	static void DeleteInterface();
};
