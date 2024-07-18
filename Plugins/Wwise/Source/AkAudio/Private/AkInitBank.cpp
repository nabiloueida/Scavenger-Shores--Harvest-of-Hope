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

#include "AkInitBank.h"

#include "AkAudioModule.h"
#include "AkSettings.h"
#include "Platforms/AkPlatformInfo.h"
#include "Wwise/WwiseResourceLoader.h"
#include "Wwise/Stats/AkAudio.h"

#if WITH_EDITORONLY_DATA
#include "Wwise/WwiseResourceCooker.h"
#endif

#if WITH_EDITORONLY_DATA
void UAkInitBank::CookAdditionalFilesOverride(const TCHAR* PackageFilename, const ITargetPlatform* TargetPlatform,
                                              TFunctionRef<void(const TCHAR* Filename, void* Data, int64 Size)> WriteAdditionalFile)
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	EnsureResourceCookerCreated(TargetPlatform);
	auto* ResourceCooker = IWwiseResourceCooker::GetForPlatform(TargetPlatform);
	if (!ResourceCooker)
	{
		return;
	}
	ResourceCooker->CookInitBank(FWwiseObjectInfo::DefaultInitBank, this, PackageFilename, WriteAdditionalFile);
}

void UAkInitBank::BeginCacheForCookedPlatformData(const ITargetPlatform* TargetPlatform)
{
	EnsureResourceCookerCreated(TargetPlatform);
}
#endif

void UAkInitBank::Serialize(FArchive& Ar)
{
	bAutoLoad = false;
	Super::Serialize(Ar);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

 #if !UE_SERVER
 #if WITH_EDITORONLY_DATA
 	if (Ar.IsCooking() && Ar.IsSaving() && !Ar.CookingTarget()->IsServerOnly())
	{
		FWwiseInitBankCookedData CookedDataToArchive;
		if (auto* ResourceCooker = IWwiseResourceCooker::GetForArchive(Ar))
		{
			ResourceCooker->PrepareCookedData(CookedDataToArchive, this, FWwiseObjectInfo::DefaultInitBank);
		}
		CookedDataToArchive.Serialize(Ar);
	 	CookedDataToArchive.SerializeBulkData(Ar, this);
	}
 #else
 	InitBankCookedData.Serialize(Ar);
 	InitBankCookedData.SerializeBulkData(Ar, this);
 #endif
 #endif
}

void UAkInitBank::UnloadInitBank(bool bAsync)
{
	auto PreviouslyLoadedInitBank = LoadedInitBank.exchange(nullptr);
	if (PreviouslyLoadedInitBank)
	{
		auto* ResourceLoader = FWwiseResourceLoader::Get();
		if (UNLIKELY(!ResourceLoader))
		{
			return;
		}

		if (bAsync)
		{
			FWwiseLoadedInitBankPromise Promise;
			Promise.EmplaceValue(MoveTemp(PreviouslyLoadedInitBank));
			ResourceUnload = ResourceLoader->UnloadInitBankAsync(Promise.GetFuture());
		}
		else
		{
			ResourceLoader->UnloadInitBank(MoveTemp(PreviouslyLoadedInitBank));
		}
	}
}

#if WITH_EDITORONLY_DATA
void UAkInitBank::PrepareCookedData()
{
	if (!IWwiseProjectDatabaseModule::ShouldInitializeProjectDatabase())
	{
		return;
	}
	auto* ResourceCooker = IWwiseResourceCooker::GetDefault();
	if (UNLIKELY(!ResourceCooker))
	{
		return;
	}
	if (!ResourceCooker->PrepareCookedData(InitBankCookedData, this, FWwiseObjectInfo::DefaultInitBank))
	{
		const auto* AudioDevice = FAkAudioDevice::Get();
		if( AudioDevice && AudioDevice->IsWwiseProfilerConnected())
		{
			UE_LOG(LogAkAudio, Verbose, TEXT("Could not fetch CookedData for Init Bank, but Wwise profiler is connected. Previous errors can be ignored."));
		}
		else
		{
			return;
		}
	}
}
#endif

TArray<FWwiseLanguageCookedData> UAkInitBank::GetLanguages()
{
#if WITH_EDITORONLY_DATA
	PrepareCookedData();
#endif

	return InitBankCookedData.Language;
}


void UAkInitBank::UnloadData(bool bAsync)
{
	UnloadInitBank(bAsync);
}

void UAkInitBank::LoadInitBank()
{
	SCOPED_AKAUDIO_EVENT_2(TEXT("LoadInitBank"));
	auto* ResourceLoader = FWwiseResourceLoader::Get();
	if (UNLIKELY(!ResourceLoader))
	{
		return;
	}
	UnloadInitBank(false);

#if WITH_EDITORONLY_DATA
	PrepareCookedData();
#endif
	
	const auto NewlyLoadedInitBank = ResourceLoader->LoadInitBank(InitBankCookedData);
	auto PreviouslyLoadedInitBank = LoadedInitBank.exchange(NewlyLoadedInitBank);
	if (UNLIKELY(PreviouslyLoadedInitBank))
	{
		ResourceLoader->UnloadInitBank(MoveTemp(PreviouslyLoadedInitBank));
	}
}


#if WITH_EDITORONLY_DATA
void UAkInitBank::MigrateWwiseObjectInfo()
{
	//Do nothing because the DefaultInitBank info is used
}

FWwiseObjectInfo* UAkInitBank::GetInfoMutable()
{
	return new FWwiseObjectInfo(FWwiseObjectInfo::DefaultInitBank.WwiseGuid, FWwiseObjectInfo::DefaultInitBank.WwiseShortId, FWwiseObjectInfo::DefaultInitBank.WwiseName);
}
#endif