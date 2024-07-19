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

#include "Wwise/WwiseAssetLibraryEditorModule.h"

class FWwiseAssetLibraryProcessor;
class FAssetTypeActions_Base_WwiseAssetLibrary;

class WWISEASSETLIBRARYEDITOR_API FWwiseAssetLibraryEditorModule : public IWwiseAssetLibraryEditorModule
{
public:
	FWwiseAssetLibraryEditorModule();
	~FWwiseAssetLibraryEditorModule();

	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual void FillTypeActions();
	virtual void RegisterTypeActions();
	virtual void UnregisterTypeActions();

	virtual FWwiseAssetLibraryPreCooker* InstantiatePreCooker(FWwiseProjectDatabase& ProjectDatabase) override;
	virtual FWwiseAssetLibraryProcessor* GetAssetLibraryProcessor() override;
	virtual FWwiseAssetLibraryProcessor* InstantiateAssetLibraryProcessor() const override;

protected:
	using SharedTypeActions = TSharedRef<FAssetTypeActions_Base_WwiseAssetLibrary>;
	TArray<SharedTypeActions> TypeActions_WwiseAssetLibrary;

	TUniquePtr<FWwiseAssetLibraryProcessor> AssetLibraryProcessor;
};

