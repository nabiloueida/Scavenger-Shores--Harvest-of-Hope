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

#include "Wwise/AssetLibrary/WwiseAssetLibraryPreCooker.h"

#include "Wwise/AssetLibrary/WwiseAssetLibrary.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryProcessor.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryFilteringSharedData.h"
#include "Wwise/Stats/AssetLibraryEditor.h"

FWwiseAssetLibraryPreCooker::FWwiseAssetLibraryPreCooker(FWwiseProjectDatabase& ProjectDatabase) :
	ProjectDatabase(ProjectDatabase)
{
}

FWwiseAssetLibraryPreCooker::~FWwiseAssetLibraryPreCooker()
{
}

FWwiseAssetLibraryPreCooker::FAssetLibraryInfoMap FWwiseAssetLibraryPreCooker::Process(const FAssetLibraryArray& AssetLibraryArray)
{
	if (AssetLibraryArray.Num() == 0)
	{
		UE_LOG(LogWwiseAssetLibraryEditor, Verbose, TEXT("FWwiseAssetLibraryPreCooker::Process: No Wwise Asset Library configured. Skip."));
		return {};
	}

	FAssetLibraryInfoMap Result;

	FWwiseAssetLibraryProcessor* Processor = FWwiseAssetLibraryProcessor::Get();
	if (!Processor)
	{
		UE_LOG(LogWwiseAssetLibraryEditor, Error, TEXT("FWwiseAssetLibraryPreCooker::Process: Could not retrieve WwiseAssetLibraryProcessor. Aborting Precooker."));
		return {};
	}

	const TUniquePtr<FWwiseAssetLibraryFilteringSharedData> FilteringSharedData { Processor->InstantiateSharedData(ProjectDatabase) };
	Processor->RetrieveAssetMap(*FilteringSharedData);

	TSet<FName> ProcessedAssetLibraries;

	for (int Iter = AssetLibraryArray.Num() - 1; Iter >= 0; --Iter)
	{
		const auto& AssetLibrarySoftPtr { AssetLibraryArray[Iter] };
		if (AssetLibrarySoftPtr.IsNull())
		{
			UE_LOG(LogWwiseAssetLibraryEditor, Verbose, TEXT("FWwiseAssetLibraryPreCooker::Process: Skipping Null Wwise Asset Library at location %d."), Iter);
			continue;
		}
		auto* AssetLibrary = AssetLibrarySoftPtr.LoadSynchronous();
		if (UNLIKELY(!AssetLibrary))
		{
			UE_LOG(LogWwiseAssetLibraryEditor, Log, TEXT("FWwiseAssetLibraryPreCooker::Process: Could not retrieve Asset Library at location %d (%s). Skipping."), Iter, *AssetLibrarySoftPtr.ToString());
			continue;
		}

		bool bIsAlreadySet;
		ProcessedAssetLibraries.Add(AssetLibrary->GetFName(), &bIsAlreadySet);
		if (UNLIKELY(bIsAlreadySet))
		{
			UE_LOG(LogWwiseAssetLibraryEditor, Warning, TEXT("FWwiseAssetLibraryPreCooker::Process: Asset Library at location %d (%s) is used twice. Skipping."), Iter, *AssetLibrarySoftPtr.ToString());
			continue;
		}
		
		Processor->FilterLibraryAssets(*FilteringSharedData, AssetLibrary->Info, Iter > 0);
		Result.Add(AssetLibrary, MakeShared<FWwiseAssetLibraryInfo>(AssetLibrary->Info));
	}
	return Result;
}
