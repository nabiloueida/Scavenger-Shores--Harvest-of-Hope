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

#include "Wwise/Metadata/WwiseMetadataSoundBank.h"
#include "Wwise/Metadata/WwiseMetadataCollections.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"
#include "Wwise/Metadata/WwiseMetadataPluginGroup.h"
#include "Wwise/Metadata/WwiseMetadataStateGroup.h"
#include "Wwise/Metadata/WwiseMetadataSwitchGroup.h"

WwiseMetadataSoundBankReference::WwiseMetadataSoundBankReference(WwiseMetadataLoader& Loader) :
	Id(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("Id"))),
	GUID(Loader.GetGuid(this, WWISE_DB_TEXT("GUID"))),
	Language(Loader.GetString(this, WWISE_DB_TEXT("Language")))
{
	Loader.LogParsed(WWISE_DB_TEXT("SoundBankReference"), Id);
}

WwiseMetadataSoundBankAttributes::WwiseMetadataSoundBankAttributes(WwiseMetadataLoader& Loader) :
	WwiseMetadataSoundBankReference(Loader),
	Align(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("Align"), WwiseRequiredMetadata::Optional)),
	bDeviceMemory(Loader.GetBool(this, WWISE_DB_TEXT("DeviceMemory"), WwiseRequiredMetadata::Optional)),
	Hash(Loader.GetGuid(this, WWISE_DB_TEXT("Hash"))),
	Type(TypeFromString(Loader.GetString(this, WWISE_DB_TEXT("Type"))))
{
	IncLoadedSize(sizeof(EMetadataSoundBankType));
	Loader.LogParsed(WWISE_DB_TEXT("SoundBankAttributes"), Id);
}

EMetadataSoundBankType WwiseMetadataSoundBankAttributes::TypeFromString(const WwiseDBString& TypeString)
{
	if (TypeString.String == WWISE_DB_TEXT("User"))
	{
		return EMetadataSoundBankType::User;
	}
	else if (TypeString.String == WWISE_DB_TEXT("Event"))
	{
		return EMetadataSoundBankType::Event;
	}
	else if (TypeString.String == WWISE_DB_TEXT("Bus"))
	{
		return EMetadataSoundBankType::Bus;
	}
	else
	{
		WWISE_DB_LOG(Warning, "WwiseMetadataSoundBankAttributes: Unknown Type: %s", *TypeString);
		return EMetadataSoundBankType::Unknown;
	}
}

WwiseMetadataSoundBank::WwiseMetadataSoundBank(WwiseMetadataLoader& Loader) :
	WwiseMetadataSoundBankAttributes(Loader),
	ObjectPath(Loader.GetString(this, WWISE_DB_TEXT("ObjectPath"))),
	ShortName(Loader.GetString(this, WWISE_DB_TEXT("ShortName"))),
	Path(Loader.GetString(this, WWISE_DB_TEXT("Path"))),
	Media(Loader.GetArray<WwiseMetadataMedia>(this, WWISE_DB_TEXT("Media"))),
	Plugins(Loader.GetObjectPtr<WwiseMetadataPluginGroup>(this, WWISE_DB_TEXT("Plugins"))),
	Events(Loader.GetArray<WwiseMetadataEvent>(this, WWISE_DB_TEXT("Events"))),
	DialogueEvents(Loader.GetArray<WwiseMetadataDialogueEvent>(this, WWISE_DB_TEXT("DialogueEvents"))),
	Busses(Loader.GetArray<WwiseMetadataBus>(this, WWISE_DB_TEXT("Busses"))),
	AuxBusses(Loader.GetArray<WwiseMetadataBus>(this, WWISE_DB_TEXT("AuxBusses"))),
	GameParameters(Loader.GetArray<WwiseMetadataGameParameter>(this, WWISE_DB_TEXT("GameParameters"))),
	StateGroups(Loader.GetArray<WwiseMetadataStateGroup>(this, WWISE_DB_TEXT("StateGroups"))),
	SwitchGroups(Loader.GetArray<WwiseMetadataSwitchGroup>(this, WWISE_DB_TEXT("SwitchGroups"))),
	Triggers(Loader.GetArray<WwiseMetadataTrigger>(this, WWISE_DB_TEXT("Triggers"))),
	ExternalSources(Loader.GetArray<WwiseMetadataExternalSource>(this, WWISE_DB_TEXT("ExternalSources"))),
	AcousticTextures(Loader.GetArray<WwiseMetadataAcousticTexture>(this, WWISE_DB_TEXT("AcousticTextures")))
{
	bIsInitBank = ShortName.String == WWISE_DB_TEXT("Init");
	Loader.LogParsed(WWISE_DB_TEXT("SoundBank"), Id);
}

WwiseDBSet<WwiseMetadataDialogueArgument> WwiseMetadataSoundBank::GetAllDialogueArguments() const
{
	WwiseDBSet<WwiseMetadataDialogueArgument> Result;
	for (const auto& DialogueEvent : DialogueEvents)
	{
		Result.Append(DialogueEvent.Arguments);
	}
	return Result;
}

WwiseDBSet<WwiseMetadataStateWithGroup> WwiseMetadataSoundBank::GetAllStates() const
{
	WwiseDBSet<WwiseMetadataStateWithGroup> Result;
	for (const auto& StateGroup : StateGroups)
	{
		for (const auto& State : StateGroup.States)
		{
			Result.Emplace(WwiseMetadataStateWithGroup(StateGroup, State), nullptr);
		}
	}
	return Result;
}

WwiseDBSet<WwiseMetadataSwitchWithGroup> WwiseMetadataSoundBank::GetAllSwitches() const
{
	WwiseDBSet<WwiseMetadataSwitchWithGroup> Result;
	for (const auto& SwitchGroup : SwitchGroups)
	{
		for (const auto& Switch : SwitchGroup.Switches)
		{
			Result.Emplace(WwiseMetadataSwitchWithGroup(SwitchGroup, Switch), nullptr);
		}
	}
	return Result;
}
