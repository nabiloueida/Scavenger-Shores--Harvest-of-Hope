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

#include "WwiseAssetLibraryInfo.generated.h"

class UWwiseAssetLibraryFilter;

/**
 * List of possible asset library references, as defined in the filtered assets
 */
UENUM()
enum class EWwiseAssetLibraryRefType
{
	Unknown,
	SoundBank,
	Media,
	ExternalSource
};

/**
 * One asset library reference towards a Wwise asset. This is the equivalent of a short-hand Wwise Project Database's AnyRef.
 */
USTRUCT()
struct WWISEASSETLIBRARYRUNTIME_API FWwiseAssetLibraryRef
{
	GENERATED_BODY()

public:
	FWwiseAssetLibraryRef() {}
	virtual ~FWwiseAssetLibraryRef() {}
	
	UPROPERTY(VisibleInstanceOnly, Category = "Default")
	EWwiseAssetLibraryRefType Type { EWwiseAssetLibraryRefType::Unknown };

	UPROPERTY(VisibleInstanceOnly, Category = "Default")
	FGuid Guid;

	UPROPERTY(VisibleInstanceOnly, Category = "Default")
	int Id { 0 };

	UPROPERTY(VisibleInstanceOnly, Category = "Default")
	FName Name;

	UPROPERTY(VisibleInstanceOnly, Category = "Advanced", meta=(EditCondition = "Type == EWwiseAssetLibraryRefType::SoundBank", EditConditionHides))
	int LanguageId { 0 };

	UPROPERTY(VisibleInstanceOnly, Category = "Advanced", meta=(EditCondition = "Type == EWwiseAssetLibraryRefType::Media", EditConditionHides))
	int SoundBankId { 0 };
};

/**
 * Information used to define the contents included in the Asset Library, to be cooked in the FWwiseAssetLibraryCookedData.
 */
USTRUCT(BlueprintType, Meta = (Category = "Wwise|AssetLibrary|Info", DisplayName = "Asset Library Info"))
struct WWISEASSETLIBRARYRUNTIME_API FWwiseAssetLibraryInfo
{
	GENERATED_BODY()

public:
	FWwiseAssetLibraryInfo() {}
	virtual ~FWwiseAssetLibraryInfo() {}
	
	/**
	 * @brief Filter chain for the assets to be applied in this library.
	 *
	 * In theory, the filtering order should have no incidence on the end result.
	 */
	UPROPERTY(EditAnywhere, Category = "Default")
	TArray<TObjectPtr<UWwiseAssetLibraryFilter>> Filters;
	
	/**
	 * @brief Filtered assets, calculated from the current filters.
	 *
	 * This is not taking into consideration other asset libraries, as prioritized in the Wwise Integrations Settings.
	 */
	UPROPERTY(VisibleInstanceOnly, Transient, Category = "Advanced")
	TArray<FWwiseAssetLibraryRef> FilteredAssets;
};
