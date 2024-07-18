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

#include "Wwise/AssetLibrary/Filters/WwiseAssetLibraryFilterLanguage.h"

#include "Wwise/AssetLibrary/WwiseAssetLibraryFilteringSharedData.h"
#include "Wwise/Stats/AssetLibraryEditor.h"

TArray<FName> UWwiseAssetLibraryFilterLanguage::GetAvailableLanguages()
{
	auto* ProjectDatabase = FWwiseProjectDatabase::Get();
	if (UNLIKELY(!ProjectDatabase) || !IWwiseProjectDatabaseModule::ShouldInitializeProjectDatabase())
	{
		return {};
	}

	TArray<FName> LanguagesInWwiseProject = { TEXT("SFX") };
	const WwiseDataStructureScopeLock DataStructure(*ProjectDatabase);
	for(auto Language : DataStructure.GetLanguages())
	{
		LanguagesInWwiseProject.Add(*Language.GetLanguageName());
	}

	return LanguagesInWwiseProject;
}
void UWwiseAssetLibraryFilterLanguage::PreFilter(
		const FWwiseAssetLibraryFilteringSharedData& Shared,
		const FWwiseAssetLibraryInfo& AssetLibraryInfo)
{
	if (SelectedLanguages.Num() == 0)
	{
		LanguageListString = TEXT("{ }");
		return;
	}

	LanguageListString = TEXT("{ ");
	auto Iter = SelectedLanguages.CreateConstIterator();
	LanguageListString.Append(*Iter);
	++Iter;
	
	for(;Iter; ++Iter)
	{
		LanguageListString.Append(TEXT(", ") + *Iter);
	}

	LanguageListString.Append(TEXT(" }"));
}

bool UWwiseAssetLibraryFilterLanguage::IsAssetAvailable(const FWwiseAssetLibraryFilteringSharedData& Shared,
		const WwiseAnyRef& Asset) const
{
	bool bMatchesLanguage = false;
	if(Asset.GetType() == WwiseRefType::Media)
	{
		auto* Media = Asset.GetMedia();
		bMatchesLanguage = SelectedLanguages.Contains(*Media->Language);
	}
	else if(Asset.GetType() == WwiseRefType::SoundBank)
	{
		auto* Bank = Asset.GetSoundBank();
		bMatchesLanguage = SelectedLanguages.Contains(*Bank->Language);
	}

	UE_LOG(LogWwiseAssetLibraryEditor, Verbose, TEXT("UWwiseAssetLibraryFilterLanguage: Asset %s %s the languages %s"),
		*Asset.GetName(), bMatchesLanguage ? TEXT("matches") : TEXT("does not match"), *LanguageListString);
	return bMatchesLanguage;
}

#define LOCTEXT_NAMESPACE "AssetTypeActions"

FText FAssetTypeActions_WwiseAssetLibraryFilterLanguage::GetName() const
{
	return LOCTEXT("AssetTypeActions_WwiseAssetLibraryFilterLanguage", "Wwise Asset Library Filter : Language");
}

UClass* FAssetTypeActions_WwiseAssetLibraryFilterLanguage::GetSupportedClass() const
{
	return UWwiseAssetLibraryFilterLanguage::StaticClass();
}

#undef LOCTEXT_NAMESPACE
