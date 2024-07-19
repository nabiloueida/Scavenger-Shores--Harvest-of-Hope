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

#include "Wwise/AssetLibrary/WwiseAssetLibrary.h"

#include "Wwise/WwiseAssetLibraryModule.h"
#include "Wwise/Stats/AssetLibrary.h"

#if !UE_SERVER && WITH_EDITORONLY_DATA 
#include "Wwise/WwiseProjectDatabaseModule.h"
#include "Wwise/WwiseResourceCooker.h"
#include "Wwise/CookedData/WwiseAssetLibraryCookedData.h"

#include "Interfaces/ITargetPlatform.h"
#else
#include "Wwise/WwiseResourceLoader.h"
#endif

UWwiseAssetLibrary::~UWwiseAssetLibrary()
{
	SCOPED_WWISEASSETLIBRARY_EVENT_3(TEXT("UWwiseAssetLibrary Dtor"));
	ResourceUnload.Wait();
}

#if WITH_EDITORONLY_DATA 
void UWwiseAssetLibrary::BeginCacheForCookedPlatformData(const ITargetPlatform* TargetPlatform)
{
	UObject::BeginCacheForCookedPlatformData(TargetPlatform);

	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

	IWwiseAssetLibraryModule::CreateResourceCookerForPlatform(TargetPlatform);
}
#endif

void UWwiseAssetLibrary::Serialize(FArchive& Ar)
{
	Super::Serialize(Ar);
	
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return;
	}

#if !UE_SERVER
#if WITH_EDITORONLY_DATA 
	if (Ar.IsCooking() && Ar.IsSaving() && !Ar.CookingTarget()->IsServerOnly())
	{
		FWwiseAssetLibraryCookedData PlatformCookedData;
		{
			if (Ar.CookingTarget() != nullptr)
			{
				IWwiseAssetLibraryModule::CreateResourceCookerForPlatform(Ar.CookingTarget());
			}
			if (auto* ResourceCooker = IWwiseResourceCooker::GetForArchive(Ar))
			{
				ResourceCooker->PrepareAssetLibrary(this, PlatformCookedData);
			}
			else
			{
				UE_LOG(LogWwiseAssetLibrary, Error, TEXT("Could not retrieve Resource Cooker for Archive. Archive will be empty."));
			}
		}
		PlatformCookedData.Serialize(Ar);

		FWwisePackagedFileSerializationOptions Options(this);
		Options.bOptional = true;		// We want to load the Bulk Data structure, but not assets in themselves. 
		
		PlatformCookedData.SerializeBulkData(Ar, Options);
	}
#else
	FWwisePackagedFileSerializationOptions Options(this);
	Options.bOptional = true;		// We want to load the Bulk Data structure, but not assets in themselves. 

	CookedData.Serialize(Ar);
	CookedData.SerializeBulkData(Ar, Options);
#endif
#endif
}

void UWwiseAssetLibrary::PostLoad()
{
	UObject::PostLoad();

	if (LIKELY(bAutoLoad))
	{
		LoadData();
	}
}

void UWwiseAssetLibrary::BeginDestroy()
{
	if (HasAnyFlags(RF_ClassDefaultObject))
	{
		return Super::BeginDestroy();
	}

	{
		SCOPED_WWISEASSETLIBRARY_EVENT_F_2(TEXT("UWwiseAssetLibrary::BeginDestroy %s"), *GetClass()->GetName());
		UE_LOG(LogWwiseAssetLibrary, Verbose, TEXT("UWwiseAssetLibrary::BeginDestroy[%p] %s %s"), this, *GetClass()->GetName(), *GetName());

		UnloadData(true);
	}
	Super::BeginDestroy();
}

void UWwiseAssetLibrary::FinishDestroy()
{
	{
		SCOPED_WWISEASSETLIBRARY_EVENT_2(TEXT("UWwiseAssetLibrary::FinishDestroy"));
		UE_LOG(LogWwiseAssetLibrary, VeryVerbose, TEXT("UWwiseAssetLibrary::FinishDestroy[%p]"), this);

		ResourceUnload.Wait();
	}
	Super::FinishDestroy();
}

void UWwiseAssetLibrary::LoadData()
{
#if !WITH_EDITORONLY_DATA
	SCOPED_WWISEASSETLIBRARY_EVENT_2(TEXT("UWwiseAssetLibrary::LoadData"));
	auto* ResourceLoader = FWwiseResourceLoader::Get();
	if (UNLIKELY(!ResourceLoader))
	{
		return;
	}

	UnloadData(false);

	const auto NewlyLoadedAssetLibrary = ResourceLoader->LoadAssetLibrary(CookedData);
	auto PreviouslyLoadedAssetLibrary = LoadedAssetLibrary.exchange(NewlyLoadedAssetLibrary);
	if (UNLIKELY(PreviouslyLoadedAssetLibrary))
	{
		ResourceLoader->UnloadAssetLibrary(MoveTemp(PreviouslyLoadedAssetLibrary));
	}
#endif
}

void UWwiseAssetLibrary::UnloadData([[maybe_unused]] bool bAsync)
{
#if !WITH_EDITORONLY_DATA
	SCOPED_WWISEASSETLIBRARY_EVENT_2(TEXT("UWwiseAssetLibrary::UnloadData"));
	auto PreviouslyLoadedAssetLibrary = LoadedAssetLibrary.exchange(nullptr);
	if (PreviouslyLoadedAssetLibrary)
	{
		auto* ResourceLoader = FWwiseResourceLoader::Get();
		if (UNLIKELY(!ResourceLoader))
		{
			return;
		}

		if (bAsync)
		{
			FWwiseLoadedAssetLibraryPromise Promise;
			Promise.EmplaceValue(MoveTemp(PreviouslyLoadedAssetLibrary));
			ResourceUnload = ResourceLoader->UnloadAssetLibraryAsync(Promise.GetFuture());
		}
		else
		{
			ResourceLoader->UnloadAssetLibrary(MoveTemp(PreviouslyLoadedAssetLibrary));
		}
	}
#endif
}
