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

#include "Wwise/WwiseEditorUtilsModuleImpl.h"

#include "AssetToolsModule.h"

IMPLEMENT_MODULE(FWwiseEditorUtilsModule, WwiseEditorUtils)

#define LOCTEXT_NAMESPACE "Wwise"

FWwiseEditorUtilsModule::FWwiseEditorUtilsModule()
{
}

void FWwiseEditorUtilsModule::StartupModule()
{
	auto& AssetTools{ FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get() };

	WwiseAssetTypeCategory = AssetTools.RegisterAdvancedAssetCategory(FName(TEXT("Wwise")), LOCTEXT("WwiseAssetCategory", "Wwise"));
	PackagingSubMenu = TArray<FText>{ LOCTEXT("WwiseAssetCategorySubmenuPackaging", "Packaging") };
}

void FWwiseEditorUtilsModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE
