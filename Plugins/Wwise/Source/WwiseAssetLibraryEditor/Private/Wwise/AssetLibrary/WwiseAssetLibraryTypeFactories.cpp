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

#include "Wwise/AssetLibrary/WwiseAssetLibraryFactories.h"

#include "Wwise/AssetLibrary/WwiseAssetLibrary.h"
#include "Wwise/WwiseEditorUtilsModule.h"

#include "UObject/ObjectMacros.h"
#include "UObject/Object.h"

UWwiseAssetLibraryFactory::UWwiseAssetLibraryFactory(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SupportedClass = UWwiseAssetLibrary::StaticClass();

	bCreateNew = true;
	bEditorImport = true;
	bEditAfterNew = true;
}

UObject* UWwiseAssetLibraryFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context,
	FFeedbackContext* Warn)
{
	return NewObject<UWwiseAssetLibrary>(InParent, Name, Flags);
}

uint32 UWwiseAssetLibraryFactory::GetMenuCategories() const
{
	if (auto* EditorUtils = IWwiseEditorUtilsModule::GetModule())
	{
		return EditorUtils->GetWwiseAssetTypeCategory();
	}
	else
	{
		return EAssetTypeCategories::Sounds;
	}
}

const TArray<FText>& UWwiseAssetLibraryFactory::GetMenuCategorySubMenus() const
{
	if (auto* EditorUtils = IWwiseEditorUtilsModule::GetModule())
	{
		return EditorUtils->GetPackagingSubMenu();
	}
	else
	{
		static TArray<FText> Submenu{};
		return Submenu;
	}
}
