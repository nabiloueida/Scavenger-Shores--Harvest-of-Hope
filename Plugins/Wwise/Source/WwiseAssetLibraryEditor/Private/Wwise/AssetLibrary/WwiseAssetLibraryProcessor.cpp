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

#include "Wwise/AssetLibrary/WwiseAssetLibraryProcessor.h"

#include "Async/ParallelFor.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryFilter.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryInfo.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryFilteringSharedData.h"
#include "Wwise/Metadata/WwiseMetadataLanguage.h"
#include "Wwise/WwiseAllowShrinking.h"

FWwiseAssetLibraryFilteringSharedData* FWwiseAssetLibraryProcessor::InstantiateSharedData(FWwiseProjectDatabase& ProjectDatabase)
{
	return new FWwiseAssetLibraryFilteringSharedData(ProjectDatabase);
}

void FWwiseAssetLibraryProcessor::RetrieveAssetMap(FWwiseAssetLibraryFilteringSharedData& Shared)
{
	const auto& SoundBanks { Shared.Db.GetSoundBanks() };
	const auto& Media { Shared.Db.GetMediaFiles() };
	const auto Num { Media.Size() + SoundBanks.Size() };
	Shared.Sources.Empty(Num);
	Shared.Remaining.Empty(Num);

	for (const auto& Item : SoundBanks)
	{
		if (!FilterSoundBank(Item.Value))
		{
			continue;
		}
		const auto Pos { Shared.Sources.Emplace(WwiseAnyRef::Create(Item.Value)) };
		Shared.Remaining.Add(Pos);
	}
	for (const auto& Item : Media)
	{
		if (!FilterMedia(Item.Value))
		{
			continue;
		}
		const auto Pos { Shared.Sources.Emplace(WwiseAnyRef::Create(Item.Value)) };
		Shared.Remaining.Add(Pos);
	}
}

bool FWwiseAssetLibraryProcessor::FilterMedia(const WwiseRefMedia& Media)
{
	if (UNLIKELY(!Media.IsValid()))
	{
		return false;
	}

	const auto Metadata { Media.GetMedia() };

	const auto Location { Metadata->Location };
	if (UNLIKELY(Location == WwiseMetadataMediaLocation::Unknown
		|| Location == WwiseMetadataMediaLocation::OtherBank))
	{
		return false;
	}

	if (Location == WwiseMetadataMediaLocation::Memory && !Metadata->bStreaming)
	{
		return false;
	}
	
	return true;
}

bool FWwiseAssetLibraryProcessor::FilterSoundBank(const WwiseRefSoundBank& SoundBank)
{
	if (UNLIKELY(!SoundBank.IsValid()))
	{
		return false;
	}

	return true;
}

void FWwiseAssetLibraryProcessor::FilterLibraryAssets(FWwiseAssetLibraryFilteringSharedData& Shared, FWwiseAssetLibraryInfo& Library, bool bUpdateRemaining)
{
	const auto Num { Shared.Remaining.Num() };
	std::atomic_int32_t FilteredAssetsNum { 0 };
	TArray<int32> FilteredPosArray;
	FilteredPosArray.AddUninitialized(Num);

	Shared.Remaining.Compact();

	for(auto Filter : Library.Filters)
	{
		if (Filter)
		{
			Filter->PreFilter(Shared, Library);
		}
	}
	
	ParallelFor(Num, [this, &Shared, &Library, &FilteredAssetsNum, &FilteredPosArray](int32 Iter)
	{
		const WwiseAnyRef& SourceRef{ Shared.Sources[Shared.Remaining[Iter]] };
		if (FilterAsset(Shared, Library, SourceRef))
		{
			const auto IdToAdd = FilteredAssetsNum++;
			FilteredPosArray[IdToAdd] = Iter;
		}
	});

	for(auto Filter : Library.Filters)
	{
		if (Filter)
		{
			Filter->PostFilter(Shared, Library);
		}
	}

	if (Num != FilteredAssetsNum)
	{
		FilteredPosArray.RemoveAt(FilteredAssetsNum, Num - FilteredAssetsNum, EWwiseAllowShrinking::No);
	}
	FilteredPosArray.Sort();

	TArray<FWwiseAssetLibraryRef>& FilteredAssets { Library.FilteredAssets };
	FilteredAssets.Reserve(FilteredAssetsNum);
	for (const auto& FilteredPos : FilteredPosArray)
	{
		const WwiseAnyRef& SourceRef{ Shared.Sources[Shared.Remaining[FilteredPos]] };
		
		FWwiseAssetLibraryRef NewRef;
		switch (SourceRef.GetType())
		{
		case WwiseRefType::Media:
			NewRef.Type = EWwiseAssetLibraryRefType::Media;
			NewRef.SoundBankId = SourceRef.GetMediaRef()->SoundBankId();
			NewRef.LanguageId = SourceRef.GetMediaRef()->LanguageId;
			break;
		case WwiseRefType::SoundBank:
			NewRef.Type = EWwiseAssetLibraryRefType::SoundBank;
			NewRef.LanguageId = SourceRef.GetSoundBankRef()->LanguageId;
			break;
		default: {}
		}
		{
			int A, B, C, D;
			SourceRef.GetGuid().GetGuidValues(A, B, C, D);
			NewRef.Guid = FGuid(A, B, C, D);
		}
		NewRef.Id = SourceRef.GetId();
		NewRef.Name = FName(*SourceRef.GetName());

		FilteredAssets.Add(NewRef);
	}

	if (bUpdateRemaining)
	{
		for (auto Iter = FilteredAssetsNum - 1; Iter >= 0; --Iter)
		{
			Shared.Remaining.RemoveAt(FilteredPosArray[Iter]);
		}
	}
}

bool FWwiseAssetLibraryProcessor::FilterAsset(const FWwiseAssetLibraryFilteringSharedData& Shared, const FWwiseAssetLibraryInfo& Library, const WwiseAnyRef& Item)
{
	const auto Num { Library.Filters.Num() };

	if (Num == 0)
	{
		return true;
	}
	if (Num == 1)
	{
		const auto Filter = Library.Filters[0];
		return UNLIKELY(!Filter) || Filter->IsAssetAvailable(Shared, Item);
	}
	if (Num == 2)
	{
		const auto Filter0 = Library.Filters[0];
		const auto Filter1 = Library.Filters[1];
		return (UNLIKELY(!Filter0) || Filter0->IsAssetAvailable(Shared, Item))
			&& (UNLIKELY(!Filter1) || Filter1->IsAssetAvailable(Shared, Item));
	}

	// Calculate all filters in parallel
	TArray<bool> Result;
	Result.AddUninitialized(Num);
	
	ParallelFor(Num, [this, &Filters = Library.Filters, &Result, &Shared, &Item](int32 Iter)
	{
		const auto Filter = Filters[Iter];
		Result[Iter] = UNLIKELY(!Filter) || Filter->IsAssetAvailable(Shared, Item);
	});
	
	for (const bool IndividualResult : Result)
	{
		if (!IndividualResult)
		{
			return false;
		}
	}
	return true;
}
