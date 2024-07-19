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

#include "Wwise/WwiseProjectDatabase.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryFilter.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryTypeActions.h"

#include "WwiseAssetLibraryTextFilter.generated.h"

UENUM()
enum class TextFilterType : uint8
{
	Name UMETA(DisplayName = "Name"),
	SystemPath UMETA(DisplayName = "System Path"),
	PathInWwise UMETA(DisplayName = "Path In Wwise")
};

USTRUCT(BlueprintType)
struct FTextFilters
{
	GENERATED_BODY()

	FTextFilters(): Type(TextFilterType::Name), IsExclusionFilter(false), UseRegex(false) {}

	FTextFilters(FString InFilter, TextFilterType InType, bool InIsExclusionFilter, bool InUseRegex) :
		Filter(InFilter), Type(InType), IsExclusionFilter(InIsExclusionFilter), UseRegex(InUseRegex) {}
	
	UPROPERTY(EditAnywhere, Category = "Default")
	FString Filter;

	UPROPERTY(EditAnywhere, Category = "Default")
	TextFilterType Type;

	/** When selected, make this filter an exclusion filter*/
	UPROPERTY(EditAnywhere, Category = "Default")
	bool IsExclusionFilter;

	UPROPERTY(EditAnywhere, Category = "Default")
	bool UseRegex;
};

UCLASS(BlueprintType, DisplayName="Filter assets based on text filter", Meta = (Category = "Wwise|AssetLibrary|Filter"))
class WWISEASSETLIBRARYEDITOR_API UWwiseAssetLibraryTextFilter : public UWwiseAssetLibraryFilter
{
	GENERATED_BODY()
	
	FString IncludedListString;
	FString ExcludedListString;

	void FilterSoundBank(const WwiseMetadataSoundBank* SoundBank, FTextFilters Filter);
	static bool PatternMatch(FTextFilters Filter, const WwiseMetadataLoadable* Metadata, const WwiseRefType AssetType);
	static bool GlobSearch(const FString& Pattern, const FString& InputText, int32 PatternIndex, int32 InputIndex);
	static FString GetInputText(const WwiseMetadataLoadable* Metadata, const TextFilterType FilterType, const WwiseRefType AssetType);
	TSet<FString> IncludedMedia;
	TSet<FString> IncludedSoundBank;
	
public:
	UWwiseAssetLibraryTextFilter() {}

	UPROPERTY(EditAnywhere, Category = "Default")
	TArray<FTextFilters> TextFilters;

	virtual void PreFilter(
		const FWwiseAssetLibraryFilteringSharedData& Shared,
		const FWwiseAssetLibraryInfo& AssetLibraryInfo) override;
	
	virtual bool IsAssetAvailable(
		const FWwiseAssetLibraryFilteringSharedData& Shared,
		const WwiseAnyRef& Asset) const override;

	virtual void PostFilter(
		const FWwiseAssetLibraryFilteringSharedData& Shared,
		const FWwiseAssetLibraryInfo& AssetLibraryInfo) override;

	void Reset()
	{
		TextFilters.Empty();
		IncludedListString.Empty();
		ExcludedListString.Empty();
	}
};

class WWISEASSETLIBRARYEDITOR_API FAssetTypeActions_WwiseAssetLibraryFilterText : public FAssetTypeActions_Base_WwiseAssetLibraryFilter
{
public:
	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
};
