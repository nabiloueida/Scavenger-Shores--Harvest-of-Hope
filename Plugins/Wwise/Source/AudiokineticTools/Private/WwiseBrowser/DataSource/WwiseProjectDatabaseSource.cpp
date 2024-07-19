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

#include "WwiseProjectDatabaseSource.h"

#include <Wwise/Stats/AudiokineticTools.h>

#include "IAudiokineticTools.h"
#include "AssetManagement/AkAssetDatabase.h"
#include "Async/Async.h"
#include "Wwise/WwiseTreeItem.h"
#include "Wwise/WwiseProjectDatabase.h"
#include "Wwise/WwiseProjectDatabaseDelegates.h"
#include "Wwise/Metadata/WwiseMetadataProjectInfo.h"

FWwiseProjectDatabaseDataSource::~FWwiseProjectDatabaseDataSource()
{
	if (OnDatabaseUpdateCompleteHandle.IsValid())
	{
		FWwiseProjectDatabaseDelegates::Get()->GetOnDatabaseUpdateCompletedDelegate().Remove(OnDatabaseUpdateCompleteHandle);
		OnDatabaseUpdateCompleteHandle.Reset();
	}
}

FText FWwiseProjectDatabaseDataSource::GetProjectName()
{
	auto* ProjectDatabase = FWwiseProjectDatabase::Get();
	if (UNLIKELY(!ProjectDatabase))
	{
		return {};
	}

	const WwiseDataStructureScopeLock DataStructure(*ProjectDatabase);
	const WwiseRefPlatform Platform = DataStructure.GetPlatform(ProjectDatabase->GetCurrentPlatform());

	if (Platform.IsValid())
	{
		if (const WwiseMetadataProjectInfo* ProjectInfo = Platform.ProjectInfo.GetProjectInfo())
		{
			return FText::FromString(*ProjectInfo->Project.Name);
		}
	}
	return {};
}

bool FWwiseProjectDatabaseDataSource::Init()
{

	OnDatabaseUpdateCompleteHandle = FWwiseProjectDatabaseDelegates::Get()->GetOnDatabaseUpdateCompletedDelegate().AddLambda([this]
	{
		AsyncTask(ENamedThreads::Type::GameThread, [this]
		{
			this->ConstructTree(true);
		});
	});

	return true;
}

bool FWwiseProjectDatabaseDataSource::ConstructTree(bool bShouldRefresh)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_2(TEXT("FWwiseProjectDatabaseDataSource::ConstructTree"))

	UE_LOG(LogAudiokineticTools, Log, TEXT("Rebuilding tree for Wwise Browser"));

	FWwiseProjectDatabase* ProjectDatabase = FWwiseProjectDatabase::Get();
	if (UNLIKELY(!ProjectDatabase))
	{
		UE_LOG(LogAudiokineticTools, Error, TEXT("ConstructTree: ProjectDatabase not loaded"));
		return false;
	}

	AllValidTreeItemsByGuid.Empty();

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems.Empty();
		RootItems.SetNum(EWwiseItemType::LastWwiseBrowserType + 1);
	}

	NodesByPath.Empty();

	{
		const WwiseDataStructureScopeLock DataStructure(*ProjectDatabase);
		if (DataStructure.GetCurrentPlatformData() == nullptr)
		{
			return false;
		}
		const WwiseEventGlobalIdsMap& Events = DataStructure.GetEvents();
		const WwiseBusGlobalIdsMap& Busses = DataStructure.GetBusses();
		const WwiseAuxBusGlobalIdsMap& AuxBusses = DataStructure.GetAuxBusses();
		const WwiseAcousticTextureGlobalIdsMap& AcousticTextures = DataStructure.GetAcousticTextures();
		const WwiseAudioDeviceGlobalIdsMap& AudioDevices = DataStructure.GetAudioDevices();
		const WwiseStateGroupGlobalIdsMap& StateGroups = DataStructure.GetStateGroups();
		const WwiseStateGlobalIdsMap& States = DataStructure.GetStates();
		const WwiseSwitchGlobalIdsMap& Switches = DataStructure.GetSwitches();
		const WwiseGameParameterGlobalIdsMap& GameParameters = DataStructure.GetGameParameters();
		const WwiseTriggerGlobalIdsMap& Triggers = DataStructure.GetTriggers();
		const WwisePluginShareSetGlobalIdsMap& EffectShareSets = DataStructure.GetPluginShareSets();
		const WwiseSwitchGroupGlobalIdsMap SwitchGroups = DataStructure.GetSwitchGroups();

		BuildEvents(Events);
		BuildBusses(Busses);
		BuildAuxBusses(AuxBusses);
		BuildAcousticTextures(AcousticTextures);
		BuildAudioDevices(AudioDevices);
		BuildStateGroups(StateGroups);
		BuildStates(States);
		BuildSwitchGroups(SwitchGroups);
		BuildSwitches(Switches);
		BuildGameParameters(GameParameters);
		BuildTriggers(Triggers);
		BuildEffectShareSets(EffectShareSets);
	}

	if(bShouldRefresh)
	{
		ProjectDatabaseDataSourceRefreshed.ExecuteIfBound();
	}
	return true;
}

FWwiseTreeItemPtr FWwiseProjectDatabaseDataSource::ConstructTreeRoot(EWwiseItemType::Type Type)
{
	ConstructTree(false);

	{
		FScopeLock AutoLock(&RootItemsLock);
		return RootItems[Type];
	}
}

int32 FWwiseProjectDatabaseDataSource::LoadChildren(
	const FGuid& InParentId, const FString& InParentPath, TArray<FWwiseTreeItemPtr>& OutChildren)
{
	FWwiseTreeItemPtr* TreeItem = NodesByPath.Find(InParentPath);

	if (TreeItem && TreeItem->IsValid())
	{
		OutChildren = TreeItem->Get()->GetChildren();
	}

	else
	{
		OutChildren = {};
	}

	return OutChildren.Num();
}

int32 FWwiseProjectDatabaseDataSource::LoadChildren(FWwiseTreeItemPtr InParentItem)
{
	if (!InParentItem)
	{
		return 0;
	}


	// For now, we have to construct the whole tree at once, so this can't be individually loaded. So we return what we have.
	return InParentItem->GetChildren().Num();
}

int32 FWwiseProjectDatabaseDataSource::GetChildItemCount(const FWwiseTreeItemPtr& InParentItem)
{
	return InParentItem->GetChildren().Num();
}

FWwiseTreeItemPtr FWwiseProjectDatabaseDataSource::GetRootItem(EWwiseItemType::Type RootType)
{
	check(RootType <= EWwiseItemType::LastWwiseBrowserType)

	if (RootType > RootItems.Num() - 1)
	{
		UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to get the Project Database Root Items. Index out of range."));
		return nullptr;
	}

	return RootItems[RootType];
}

FWwiseTreeItemPtr FWwiseProjectDatabaseDataSource::LoadFilteredRootItem(EWwiseItemType::Type ItemType,
	TSharedPtr<StringFilter> CurrentFilter)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::LoadFilteredRootItem"))
	check(CurrentFilter.IsValid())

	FWwiseTreeItemPtr CurrentTreeRootItem;

	{
		FScopeLock AutoLock(&RootItemsLock);
		if (ItemType > RootItems.Num())
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to get the Project Database Root Items. Index out of range."));
			return nullptr;
		}

		CurrentTreeRootItem = RootItems[ItemType];
		FString CurrentFilterText = CurrentFilter->GetRawFilterText().ToString();

		if (!CurrentFilterText.IsEmpty() && CurrentTreeRootItem.IsValid())
		{
			FWwiseTreeItemPtr FilteredTreeRootItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::BrowserDisplayNames[ItemType],
				CurrentTreeRootItem->FolderPath, nullptr, EWwiseItemType::Folder, FGuid(CurrentTreeRootItem->ItemId), CurrentTreeRootItem->ShortId);
			FilteredTreeRootItem->ChildCountInWwise = CurrentTreeRootItem->ChildCountInWwise;

			if (!OldFilterText.IsEmpty() && CurrentFilterText.StartsWith(OldFilterText))
			{
				CopyTree(CurrentTreeRootItem, FilteredTreeRootItem);
			}

			else
			{
				CopyTree(RootItems[ItemType], FilteredTreeRootItem);
			}

			FilterTree(FilteredTreeRootItem, CurrentFilter);
			OldFilterText = CurrentFilterText;
			return FilteredTreeRootItem;
		}

		if (RootItems[ItemType])
		{
			return RootItems[ItemType];
		}

		return nullptr;
	}
}

FWwiseTreeItemPtr FWwiseProjectDatabaseDataSource::FindItemFromPath(const FString& InCurrentItemPath)
{
	FWwiseTreeItemPtr* FoundItem = NodesByPath.Find(InCurrentItemPath);
	if (FoundItem)
	{
		return *FoundItem;
	}

	return nullptr;
}

FWwiseTreeItemPtr FWwiseProjectDatabaseDataSource::FindItem(const FWwiseTreeItemPtr InItem)
{
	FWwiseTreeItemPtr* FoundItem = AllValidTreeItemsByGuid.Find(InItem->ItemId);
	if (FoundItem)
	{
		return *FoundItem;
	}

	for(auto TreeItem : AllValidTreeItemsByGuid)
	{
		if (TreeItem.Value->ShortId == InItem->ShortId && InItem->ShortId > 0)
		{
			return TreeItem.Value;
		}
		if(TreeItem.Value->DisplayName == InItem->DisplayName)
		{
			return TreeItem.Value;
		}
	}
	return nullptr;
}

void FWwiseProjectDatabaseDataSource::BuildEvents(const WwiseEventGlobalIdsMap& Events)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildEvents"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::EventsBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::Event], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::Event] = FolderItem;
	}

	for (const auto& Event : Events)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefEvent> EventPair(Event);
		const auto& WwiseItem = EventPair.GetSecond().GetEvent();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Event Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		WwiseMetadataBasicReference EventRef = WwiseMetadataBasicReference(WwiseItem->Id, WwiseItem->Name, WwiseItem->ObjectPath, WwiseItem->GUID);
		if (!BuildFolderHierarchy(EventRef, EWwiseItemType::Event, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();

}

void FWwiseProjectDatabaseDataSource::BuildBusses(const WwiseBusGlobalIdsMap& Busses)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildBusses"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::BussesBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::AuxBus], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::AuxBus] = FolderItem;
	}

	for (auto& Bus : Busses)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefBus> BusPair(Bus);
		const auto& WwiseItem = BusPair.GetSecond().GetBus();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Bus Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::Bus, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildAuxBusses(const WwiseAuxBusGlobalIdsMap& AuxBusses)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildEvents"))

	FWwiseTreeItemPtr FolderItem;

	{
		FScopeLock AutoLock(&RootItemsLock);
		FolderItem = RootItems[EWwiseItemType::AuxBus];
	}

	for (const auto& AuxBus : AuxBusses)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefAuxBus> AuxBusPair(AuxBus);
		const auto& WwiseItem = AuxBusPair.GetSecond().GetAuxBus();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Aux Bus Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::AuxBus, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}
}

void FWwiseProjectDatabaseDataSource::BuildAcousticTextures(const WwiseAcousticTextureGlobalIdsMap& AcousticTextures)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildAcousticTextures"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::AcousticTexturesBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::AcousticTexture], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::AcousticTexture] = FolderItem;
	}

	for (const auto& AcousticTexture : AcousticTextures)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefAcousticTexture> AcousticTexturePair(AcousticTexture);
		const WwiseMetadataAcousticTexture* WwiseItem = AcousticTexturePair.GetSecond().GetAcousticTexture();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Acoustic Texture Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::AcousticTexture, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildAudioDevices(const WwiseAudioDeviceGlobalIdsMap& AudioDevices)
{
	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::AudioDeviceShareSetBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::AudioDeviceShareSet], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::AudioDeviceShareSet] = FolderItem;
	}

	for (const auto& AudioDevice : AudioDevices)
	{
		const auto& WwiseItem = AudioDevice.Value.GetPlugin();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Audio Device Name: %s"), *WwiseItem->Name);

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::AudioDeviceShareSet, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Picker"), *WwiseItem->ObjectPath);
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildStateGroups(const WwiseStateGroupGlobalIdsMap& StateGroups)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildStateGroups"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::StatesBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::State], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::State] = FolderItem;
	}

	for (const auto& StateGroup : StateGroups)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefStateGroup> StateGroupPair(StateGroup);
		const auto& WwiseItem = StateGroupPair.GetSecond().GetStateGroup();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("State Group Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::StateGroup, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildStates(const WwiseStateGlobalIdsMap& States)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildEvents"))

	FWwiseTreeItemPtr StateGroupFolderItem;

	{
		FScopeLock AutoLock(&RootItemsLock);
		StateGroupFolderItem = RootItems[EWwiseItemType::State];
	}

	for (const auto& State : States)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableGroupValueKey, WwiseRefState> StatePair(State);
		const auto& WwiseItem = StatePair.GetSecond().GetState();

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::State, StateGroupFolderItem, StatePair.GetSecond().GetStateGroup()->Id))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}
}

void FWwiseProjectDatabaseDataSource::BuildSwitchGroups(const WwiseSwitchGroupGlobalIdsMap& SwitchGroups)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildEvents"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::SwitchesBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::Switch], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::Switch] = FolderItem;
	}

	for (const auto& SwitchGroup : SwitchGroups)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefSwitchGroup> SwitchGroupPair(SwitchGroup);
		const auto& WwiseItem = SwitchGroupPair.GetSecond().GetSwitchGroup();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Switch Group Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::SwitchGroup, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildSwitches(const WwiseSwitchGlobalIdsMap& Switches)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildSwitchs"))

	FWwiseTreeItemPtr SwitchGroupFolderItem;

	{
		FScopeLock AutoLock(&RootItemsLock);
		SwitchGroupFolderItem = RootItems[EWwiseItemType::Switch];
	}

	for (const auto& Switch : Switches)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableGroupValueKey, WwiseRefSwitch> SwitchPair(Switch);
		const auto& WwiseItem = SwitchPair.GetSecond().GetSwitch();

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::Switch, SwitchGroupFolderItem, SwitchPair.GetSecond().GetSwitchGroup()->Id))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}
}

void FWwiseProjectDatabaseDataSource::BuildGameParameters(const WwiseGameParameterGlobalIdsMap& GameParameters)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildGameParameters"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::GameParametersBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::GameParameter], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::GameParameter] = FolderItem;
	}

	for (const auto& GameParameter : GameParameters)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefGameParameter> GameParameterPair(GameParameter);
		const WwiseMetadataGameParameter* WwiseItem = GameParameterPair.GetSecond().GetGameParameter();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("GameParameter Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::GameParameter, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildTriggers(const WwiseTriggerGlobalIdsMap& Triggers)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildTriggers"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::TriggersBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::Trigger], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::Trigger] = FolderItem;
	}

	for (const auto& Trigger : Triggers)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefTrigger> TriggerPair(Trigger);
		const auto& WwiseItem = TriggerPair.GetSecond().GetTrigger();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("Trigger Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::Trigger, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

void FWwiseProjectDatabaseDataSource::BuildEffectShareSets(const WwisePluginShareSetGlobalIdsMap& EffectShareSets)
{
	SCOPED_AUDIOKINETICTOOLS_EVENT_3(TEXT("FWwiseProjectDatabaseDataSource::BuildEffectShareSets"))

	const auto& FolderItem = MakeShared<FWwiseTreeItem>(EWwiseItemType::ShareSetsBrowserName, TEXT("\\") + EWwiseItemType::FolderNames[EWwiseItemType::EffectShareSet], nullptr, EWwiseItemType::Folder, FGuid());
	NodesByPath.Add(FolderItem->FolderPath, FolderItem);

	{
		FScopeLock AutoLock(&RootItemsLock);
		RootItems[EWwiseItemType::EffectShareSet] = FolderItem;
	}

	for (const auto& EffectShareSet : EffectShareSets)
	{
		WwiseDBPair<const WwiseDatabaseLocalizableIdKey, WwiseRefPluginShareSet> EffectShareSetPair(EffectShareSet);
		const auto& WwiseItem = EffectShareSetPair.GetSecond().GetPlugin();
		UE_LOG(LogAudiokineticTools, VeryVerbose, TEXT("ShareSet Name: %s"), *FWwiseStringConverter::ToFString(WwiseItem->Name));

		if (!BuildFolderHierarchy(*WwiseItem, EWwiseItemType::EffectShareSet, FolderItem))
		{
			UE_LOG(LogAudiokineticTools, Error, TEXT("Failed to place %s in the Wwise Browser"), *FWwiseStringConverter::ToFString(WwiseItem->ObjectPath));
		}
	}

	FolderItem->ChildCountInWwise = FolderItem->GetChildren().Num();
}

bool FWwiseProjectDatabaseDataSource::ParseTreePath(const FString& ObjectPath, WwiseItemTreePath& OutItemPath)
{
	TArray<FString> SwitchPartsArray;
	ObjectPath.ParseIntoArray(SwitchPartsArray, TEXT("\\"));

	if (SwitchPartsArray.Num() < 3)
	{
		UE_LOG(LogAudiokineticTools, Warning, TEXT("Failed to determine object hierarchy from path: %s"), *ObjectPath);
		return false;
	}

	OutItemPath.HierarchyName = SwitchPartsArray[0];
	OutItemPath.RootFolder = SwitchPartsArray[1];
	OutItemPath.ItemName = SwitchPartsArray[SwitchPartsArray.Num() - 1];

	for (int i = 2; i < SwitchPartsArray.Num() - 1; ++i)
	{
		OutItemPath.IntermediateFolders.Add(SwitchPartsArray[i]);
	}

	return true;
}

bool FWwiseProjectDatabaseDataSource::IsContainer(EWwiseItemType::Type ItemType) const
{
	return ItemType == EWwiseItemType::Bus || ItemType == EWwiseItemType::SwitchGroup || ItemType ==
		EWwiseItemType::StateGroup || ItemType == EWwiseItemType::AuxBus || ItemType == EWwiseItemType::MotionBus;
}

bool FWwiseProjectDatabaseDataSource::BuildFolderHierarchy(
	const WwiseMetadataBasicReference& WwiseItem, EWwiseItemType::Type
	ItemType, const FWwiseTreeItemPtr CurrentRootFolder, uint32 GroupId)
{
	const FString ItemPath = *WwiseItem.ObjectPath;
	WwiseItemTreePath TreePath;
	FWwiseTreeItemPtr ParentItem = CurrentRootFolder;

	if (ParseTreePath(ItemPath, TreePath))
	{
		FString CurrentPath = "\\" + TreePath.HierarchyName;
		TArray<FString> AllFolders = TArray<FString>{ TreePath.RootFolder };
		AllFolders.Append(TreePath.IntermediateFolders);

		for (FString FolderName : AllFolders)
		{
			CurrentPath.Append("\\").Append(FolderName);

			FWwiseTreeItemPtr FolderItem;
			FWwiseTreeItemPtr* FolderItemPtr = NodesByPath.Find(CurrentPath);

			if (!FolderItemPtr)
			{
				FolderItem = MakeShared<FWwiseTreeItem>(FolderName, CurrentPath, ParentItem,
					EWwiseItemType::Folder, FGuid());

				NodesByPath.Add(CurrentPath, FolderItem);
				ParentItem->AddChild(FolderItem);
				ParentItem->ChildCountInWwise = ParentItem->GetChildren().Num();
			}
			else
			{
				FolderItem = *FolderItemPtr;
			}

			ParentItem = FolderItem;
		}

		int A, B, C, D;
		WwiseItem.GUID.GetGuidValues(A, B, C, D);
		FGuid Guid(A, B, C, D);
		if (!AllValidTreeItemsByGuid.Find(Guid))
		{
			if(auto TreeItemPtr = NodesByPath.Find(*FWwiseStringConverter::ToFString(WwiseItem.ObjectPath)))
			{
				auto TreeItem = *TreeItemPtr;
				TreeItem->ItemType = ItemType;
				TreeItem->ItemId = Guid;
				TreeItem->ShortId = WwiseItem.Id;
				TreeItem->GroupId = GroupId;
			}
			else
			{
				const auto& NewWwiseTreeItem = MakeShared<FWwiseTreeItem>(WwiseItem, ParentItem, ItemType);
				NewWwiseTreeItem->ShortId = WwiseItem.Id;
				NewWwiseTreeItem->GroupId = GroupId;
				ParentItem->AddChild(NewWwiseTreeItem);
				AllValidTreeItemsByGuid.Add(NewWwiseTreeItem->ItemId, NewWwiseTreeItem);

				if (IsContainer(ItemType))
				{
					NodesByPath.Add(FWwiseStringConverter::ToFString(WwiseItem.ObjectPath), NewWwiseTreeItem);
				}
			}
		}

		return true;
	}
	return false;
}

void FWwiseProjectDatabaseDataSource::CopyTree(FWwiseTreeItemPtr SourceTreeItem, FWwiseTreeItemPtr DestTreeItem)
{
	for (auto& CurrItem: SourceTreeItem->GetChildren())
	{
		FWwiseTreeItemPtr NewItem = MakeShared<FWwiseTreeItem>(CurrItem->DisplayName, CurrItem->FolderPath, CurrItem->Parent.Pin(), CurrItem->ItemType, CurrItem->ItemId, CurrItem->ShortId);
		NewItem->WwiseItemRef = CurrItem->WwiseItemRef;
		DestTreeItem->AddChild(NewItem);
		DestTreeItem->ChildCountInWwise = SourceTreeItem->ChildCountInWwise;

		CopyTree(CurrItem, NewItem);
	}
}

void FWwiseProjectDatabaseDataSource::FilterTree(FWwiseTreeItemPtr TreeItem, TSharedPtr<StringFilter> SearchFilter)
{
	TArray<FWwiseTreeItemPtr> ItemsToRemove;
	for (auto& CurrItem: TreeItem->GetChildren())
	{
		FilterTree(CurrItem, SearchFilter);

		if (!SearchFilter->PassesFilter(CurrItem->DisplayName) && CurrItem->GetChildren().Num() == 0)
		{
			ItemsToRemove.Add(CurrItem);
		}
	}

	for (int32 i = 0; i < ItemsToRemove.Num(); i++)
	{
		TreeItem->RemoveChild(ItemsToRemove[i]);
	}

	TreeItem->ChildCountInWwise = TreeItem->GetChildren().Num();
}

