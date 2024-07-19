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

#include "Modules/ModuleManager.h"
#include "Misc/ConfigCacheIni.h"

class IWwiseAssetLibraryModule : public IModuleInterface
{
public:
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

	static IWwiseAssetLibraryModule* GetModule()
	{
		static IWwiseAssetLibraryModule* Module = nullptr;
		if (LIKELY(!IsEngineExitRequested()) && LIKELY(Module))
		{
			return Module;
		}
		
		const auto ModuleName = GetModuleName();
		if (ModuleName.IsNone())
		{
			return nullptr;
		}

		FModuleManager& ModuleManager = FModuleManager::Get();
		Module = ModuleManager.GetModulePtr<IWwiseAssetLibraryModule>(ModuleName);
		if (UNLIKELY(!Module))
		{
			if (UNLIKELY(IsEngineExitRequested()))
			{
				UE_LOG(LogLoad, Log, TEXT("Skipping reloading missing WwiseAssetLibrary module: Exiting."));
			}
			else if (UNLIKELY(!IsInGameThread()))
			{
				UE_LOG(LogLoad, Warning, TEXT("Skipping loading missing WwiseAssetLibrary module: Not in game thread"));
			}
			else
			{
				UE_LOG(LogLoad, Log, TEXT("Loading WwiseAssetLibrary module: %s"), *ModuleName.GetPlainNameString());
				Module = ModuleManager.LoadModulePtr<IWwiseAssetLibraryModule>(ModuleName);
				if (UNLIKELY(!Module))
				{
					UE_LOG(LogLoad, Fatal, TEXT("Could not load WwiseAssetLibrary module: %s not found"), *ModuleName.GetPlainNameString());
				}
			}
		}

		return Module;
	}

#if WITH_EDITORONLY_DATA
	static void CreateResourceCookerForPlatform(const ITargetPlatform* TargetPlatform)
	{
		if (const auto* Module = GetModule())
		{
			Module->DoCreateResourceCookerForPlatform(TargetPlatform);
		}
	}
	virtual void DoCreateResourceCookerForPlatform(const ITargetPlatform* TargetPlatform) const = 0;
	virtual void SetCreateResourceCookerForPlatformFct(const TFunction<void(const ITargetPlatform* TargetPlatform)>& Func) = 0;
#endif

private:
	static inline FName GetModuleNameFromConfig()
	{
		FString ModuleName = TEXT("WwiseAssetLibrary");
		GConfig->GetString(TEXT("Audio"), TEXT("WwiseAssetLibraryModuleName"), ModuleName, GEngineIni);
		return FName(ModuleName);
	}
};
