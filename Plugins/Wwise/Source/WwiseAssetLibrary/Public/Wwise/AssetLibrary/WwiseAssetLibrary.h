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
#include "Misc/CoreMisc.h"

#if WITH_EDITORONLY_DATA
#include "Wwise/AssetLibrary/WwiseAssetLibraryInfo.h"
#else
#include "Wwise/CookedData/WwiseAssetLibraryCookedData.h"
#endif

#include "Wwise/WwiseResourceLoaderFuture.h"
#include "Wwise/Loaded/WwiseLoadedAssetLibrary.h"

#include "WwiseAssetLibrary.generated.h"

/**
 * This is just Wwise SoundBanks but with extra steps.
 */
UCLASS(BlueprintType, Category="Wwise", DisplayName="Wwise Asset Library", EditInlineNew)
class WWISEASSETLIBRARY_API UWwiseAssetLibrary : public UObject
{
	GENERATED_BODY()

public:
	/// When true, SoundBanks and media associated with this asset are loaded in the Wwise SoundEngine when Unreal loads this asset.
	UPROPERTY(EditAnywhere, Category = "Default")
	bool bAutoLoad{ true };


	UWwiseAssetLibrary() {}
	virtual ~UWwiseAssetLibrary() override;

#if WITH_EDITORONLY_DATA 
	virtual void BeginCacheForCookedPlatformData(const ITargetPlatform* TargetPlatform);
#endif
	
	virtual void Serialize(FArchive& Ar) override;
	virtual void PostLoad() override;
	virtual void BeginDestroy() override;
	virtual void FinishDestroy() override;

	virtual bool IsPostLoadThreadSafe() const override { return true; }
	virtual bool IsDestructionThreadSafe() const override { return true; }

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|AkAudioType")
	virtual void LoadData();

	UFUNCTION(BlueprintCallable, Category = "Audiokinetic|AkAudioType")
	virtual void UnloadData(bool bAsync = false);


#if WITH_EDITORONLY_DATA
	UPROPERTY(Blueprintable, EditAnywhere, Category = "Default")
	FWwiseAssetLibraryInfo Info;
#else
	FWwiseLoadedAssetLibraryPtrAtomic LoadedAssetLibrary{ nullptr };
	FWwiseAssetLibraryCookedData CookedData;
#endif

protected:
	FWwiseResourceUnloadFuture ResourceUnload;
};
