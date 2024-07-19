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

#include "Wwise/AssetLibrary/Filters/WwiseAssetLibraryTextFilter.h"

#include "Internationalization/Regex.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryFilteringSharedData.h"
#include "Wwise/Stats/AssetLibraryEditor.h"

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_WwiseAssetLibraryFilterText::GetName() const
{
	return LOCTEXT("AssetTypeActions_WwiseAssetLibraryFilterText", "Wwise Asset Library Filter : Text");
}

UClass* FAssetTypeActions_WwiseAssetLibraryFilterText::GetSupportedClass() const
{
	return UWwiseAssetLibraryTextFilter::StaticClass();
}

#undef LOCTEXT_NAMESPACE 

void UWwiseAssetLibraryTextFilter::FilterSoundBank(const WwiseMetadataSoundBank* SoundBank, FTextFilters Filter)
{
	if(PatternMatch(Filter, &*SoundBank, WwiseRefType::SoundBank))
	{
		FString ShortName = *SoundBank->ShortName;
		IncludedSoundBank.Add(ShortName);
	}
	for (auto Media : SoundBank->Media)
	{
		if(PatternMatch(Filter, &Media, WwiseRefType::Media))
		{
			FString ShortName = *Media.ShortName;
			IncludedMedia.Add(ShortName);
		}
	}
}

bool UWwiseAssetLibraryTextFilter::PatternMatch(FTextFilters Filter, const WwiseMetadataLoadable* Metadata, const WwiseRefType AssetType)
{
	const FString InputText = GetInputText(Metadata, Filter.Type, AssetType);
	if(Filter.UseRegex)
	{
		//Empty filter is invalid
		if(Filter.Filter.IsEmpty())
		{
			return false;
		}
		FRegexMatcher Matcher(FRegexPattern(Filter.Filter), InputText);
		return Matcher.FindNext();
	}
	return GlobSearch(Filter.Filter, InputText, 0, 0);
}

bool UWwiseAssetLibraryTextFilter::GlobSearch(const FString& Pattern, const FString& InputText, int32 PatternIndex, int32 InputIndex)
{
	while(PatternIndex < Pattern.Len())
	{
		const auto currentChar = Pattern[PatternIndex];
		if(currentChar == '?')
		{
			if(InputIndex >= InputText.Len())
				return false;
			++PatternIndex;
			++InputIndex;
		}
		else if(currentChar == '*')
		{
			if(GlobSearch(Pattern, InputText, PatternIndex + 1, InputIndex) ||
				(InputIndex < InputText.Len() && GlobSearch(Pattern, InputText, PatternIndex, InputIndex+1)))
				return true;
			return false;
		}
		else
		{
			if(InputIndex >= InputText.Len() || currentChar != InputText[InputIndex])
				return false;
			++PatternIndex;
			++InputIndex;
		}
	}
	return InputIndex == InputText.Len();
}

FString UWwiseAssetLibraryTextFilter::GetInputText(const WwiseMetadataLoadable* Metadata,
	const TextFilterType FilterType, const WwiseRefType AssetType)
{
	FString InputText;
	switch (FilterType)
	{
	case TextFilterType::Name:
		if(AssetType == WwiseRefType::Media)
		{
			InputText = *(static_cast<const WwiseMetadataMedia*>(Metadata)->ShortName);
		}
		else if(AssetType == WwiseRefType::SoundBank)
		{
			InputText = *(static_cast<const WwiseMetadataSoundBank*>(Metadata)->ShortName);
		}
		break;
	case TextFilterType::SystemPath:
		if(AssetType == WwiseRefType::Media)
		{
			InputText = *(static_cast<const WwiseMetadataMedia*>(Metadata)->Path);
		}
		else if(AssetType == WwiseRefType::SoundBank)
		{
			InputText = *(static_cast<const WwiseMetadataSoundBank*>(Metadata)->Path);
		}
		break;
	case TextFilterType::PathInWwise:
		if(AssetType == WwiseRefType::SoundBank)
		{
			InputText = *(static_cast<const WwiseMetadataSoundBank*>(Metadata)->ObjectPath);
		}
		break;

	}
	return InputText;
}

void UWwiseAssetLibraryTextFilter::PreFilter(
		const FWwiseAssetLibraryFilteringSharedData& Shared,
		const FWwiseAssetLibraryInfo& AssetLibraryInfo)
{
	if (TextFilters.Num() == 0)
	{
		ExcludedListString = TEXT("{ }");
		IncludedListString = TEXT("{ }");
		return;
	}

	ExcludedListString = TEXT("{ ");
	IncludedListString = TEXT("{ ");
	auto Iter = TextFilters.CreateConstIterator();
	FString Filter = Iter->Filter;
	if(Iter->IsExclusionFilter)
	{
		ExcludedListString.Append(*Filter);
	}
	else
	{
		IncludedListString.Append(*Filter);
	}
	++Iter;
	
	for(;Iter; ++Iter)
	{
		Filter = Iter->Filter;
		if(Iter->IsExclusionFilter)
		{
			ExcludedListString.Append(Filter);
		}
		else
		{
			IncludedListString.Append(Filter);
		}
	}

	ExcludedListString.Append(TEXT(" }"));
	IncludedListString.Append(TEXT(" }"));

	//Filter from Event asset type
	const WwiseEventGlobalIdsMap& Events = Shared.Db.GetEvents();
	for (auto Event : Events)
	{
		for(const auto TextFilter : TextFilters)
		{
			const auto SoundBank = Event.Value.GetSoundBank();
			FilterSoundBank(SoundBank, TextFilter);
		}
	}
	//Filter from AuxBus asset type
	const WwiseAuxBusGlobalIdsMap& AuxBusses = Shared.Db.GetAuxBusses();
	for (auto AuxBus : AuxBusses)
	{
		for(const auto TextFilter : TextFilters)
		{
			const auto SoundBank = AuxBus.Value.GetSoundBank();
			FilterSoundBank(SoundBank, TextFilter);
		}
	}
}

bool UWwiseAssetLibraryTextFilter::IsAssetAvailable(const FWwiseAssetLibraryFilteringSharedData& Shared,
		const WwiseAnyRef& Asset) const
{
	const WwiseMetadataLoadable* Metadata;
	const WwiseRefType AssetType = Asset.GetType();
	FString AssetName;
	if(AssetType == WwiseRefType::Media)
	{
		Metadata = Asset.GetMedia();
		AssetName = *(static_cast<const WwiseMetadataMedia*>(Metadata)->ShortName);
	}
	else if(AssetType == WwiseRefType::SoundBank)
	{
		Metadata = Asset.GetSoundBank();
		AssetName = *(static_cast<const WwiseMetadataSoundBank*>(Metadata)->ShortName);
	}
	else
	{
		return false;
	}
	
	for(auto Filter : TextFilters)
	{
		if((AssetType == WwiseRefType::Media || AssetType == WwiseRefType::SoundBank) && !Filter.IsExclusionFilter)
		{
			if(AssetType == WwiseRefType::Media ? IncludedMedia.Contains(AssetName) : IncludedSoundBank.Contains(AssetName))
			{
				return true;
			}
		}
		if(PatternMatch(Filter, Metadata, AssetType))
		{
			if(Filter.IsExclusionFilter)
			{
				UE_LOG(LogWwiseAssetLibraryEditor, Verbose, TEXT("UWwiseAssetLibraryFilterText: Asset %s matches at least one of the following exclusion expression %s and was excluded."), *AssetName, *ExcludedListString);
			}
			else
			{
				UE_LOG(LogWwiseAssetLibraryEditor, Verbose, TEXT("UWwiseAssetLibraryFilterText: Asset %s matches at least one of the following inclusion expression %s and was included."), *AssetName, *IncludedListString);
			}
			return !Filter.IsExclusionFilter;
		}
	}
	UE_LOG(LogWwiseAssetLibraryEditor, Verbose, TEXT("UWwiseAssetLibraryFilterText: Asset %s does not match at least one of the following inclusion expression %s and was excluded."), *AssetName, *IncludedListString);
	return false;
}

void UWwiseAssetLibraryTextFilter::PostFilter(const FWwiseAssetLibraryFilteringSharedData& Shared,
	const FWwiseAssetLibraryInfo& AssetLibraryInfo)
{
	IncludedMedia.Empty();
	IncludedSoundBank.Empty();
}
