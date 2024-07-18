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

#include "Wwise/WwiseAssetLibraryEditorModuleImpl.h"

#include "Wwise/AssetLibrary/WwiseAssetLibraryPreCooker.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryProcessor.h"
#include "Wwise/AssetLibrary/WwiseAssetLibraryTypeActions.h"

#include "AssetToolsModule.h"
#include "IAssetTools.h"

IMPLEMENT_MODULE(FWwiseAssetLibraryEditorModule, WwiseAssetLibraryEditor)

FWwiseAssetLibraryEditorModule::FWwiseAssetLibraryEditorModule()
{
}

inline FWwiseAssetLibraryEditorModule::~FWwiseAssetLibraryEditorModule()
{
}

void FWwiseAssetLibraryEditorModule::StartupModule()
{
	RegisterTypeActions();
}

void FWwiseAssetLibraryEditorModule::ShutdownModule()
{
	UnregisterTypeActions();
}

void FWwiseAssetLibraryEditorModule::FillTypeActions()
{
	TypeActions_WwiseAssetLibrary.Emplace( MakeShared<FAssetTypeActions_WwiseAssetLibrary>() );
}

void FWwiseAssetLibraryEditorModule::RegisterTypeActions()
{
	FillTypeActions();
	
	IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
	for(const auto& TypeAction : TypeActions_WwiseAssetLibrary)
	{
		AssetTools.RegisterAssetTypeActions(TypeAction);
	}
}

void FWwiseAssetLibraryEditorModule::UnregisterTypeActions()
{
	if (FAssetToolsModule::IsModuleLoaded())
	{
		IAssetTools& AssetTools = FAssetToolsModule::GetModule().Get();
		for(const auto& TypeAction : TypeActions_WwiseAssetLibrary)
		{
			AssetTools.UnregisterAssetTypeActions(TypeAction);
		}
	}
	TypeActions_WwiseAssetLibrary.Reset();
}

FWwiseAssetLibraryPreCooker* FWwiseAssetLibraryEditorModule::InstantiatePreCooker(FWwiseProjectDatabase& ProjectDatabase)
{
	return new FWwiseAssetLibraryPreCooker(ProjectDatabase);
}

FWwiseAssetLibraryProcessor* FWwiseAssetLibraryEditorModule::GetAssetLibraryProcessor()
{
	if (!AssetLibraryProcessor.IsValid())
	{
		AssetLibraryProcessor.Reset(InstantiateAssetLibraryProcessor());
	}
	return AssetLibraryProcessor.Get();
}

FWwiseAssetLibraryProcessor* FWwiseAssetLibraryEditorModule::InstantiateAssetLibraryProcessor() const
{
	return new FWwiseAssetLibraryProcessor;
}

