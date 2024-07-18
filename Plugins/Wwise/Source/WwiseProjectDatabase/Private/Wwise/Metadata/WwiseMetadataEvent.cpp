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

#include "Wwise/Metadata/WwiseMetadataEvent.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"
#include "Wwise/Metadata/WwiseMetadataPluginGroup.h"

WwiseMetadataEventReference::WwiseMetadataEventReference(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader),
	MaxAttenuation(Loader.GetFloat(this, WWISE_DB_TEXT("MaxAttenuation"), WwiseRequiredMetadata::Optional)),
	DurationType(DurationTypeFromString(Loader.GetString(this, WWISE_DB_TEXT("DurationType")))),
	DurationMin(Loader.GetFloat(this, WWISE_DB_TEXT("DurationMin"), WwiseRequiredMetadata::Optional)),
	DurationMax(Loader.GetFloat(this, WWISE_DB_TEXT("DurationMax"), WwiseRequiredMetadata::Optional))
{
	//IncLoadedSize(sizeof(WwiseMetadataEventDurationType));
	Loader.LogParsed(WWISE_DB_TEXT("EventReference"), Id, Name);
}

WwiseMetadataEventDurationType WwiseMetadataEventReference::DurationTypeFromString(const WwiseDBString& TypeString)
{
	if (TypeString == WWISE_DB_TEXT("OneShot"))
	{
		return WwiseMetadataEventDurationType::OneShot;
	}
	else if (TypeString == WWISE_DB_TEXT("Infinite"))
	{
		return WwiseMetadataEventDurationType::Infinite;
	}
	else if (TypeString == WWISE_DB_TEXT("Mixed"))
	{
		return WwiseMetadataEventDurationType::Mixed;
	}
	else if (!(TypeString == WWISE_DB_TEXT("Unknown")))
	{
		WWISE_DB_LOG(Warning, "WwiseMetadataEventReference: Unknown DurationType: %s", *TypeString);
	}
	return WwiseMetadataEventDurationType::Unknown;
}

WwiseMetadataEvent::WwiseMetadataEvent(WwiseMetadataLoader& Loader) :
	WwiseMetadataEventReference(Loader),
	MediaRefs(Loader.GetArray<WwiseMetadataMediaReference>(this, WWISE_DB_TEXT("MediaRefs"))),
	ExternalSourceRefs(Loader.GetArray<WwiseMetadataExternalSourceReference>(this, WWISE_DB_TEXT("ExternalSourceRefs"))),
	PluginRefs(Loader.GetObjectPtr<WwiseMetadataPluginReferenceGroup>(this, WWISE_DB_TEXT("PluginRefs"))),
	ActionSetFX(Loader.GetArray<WwiseMetadataActionSetFXEntry>(this, WWISE_DB_TEXT("ActionSetFX"))),
	AuxBusRefs(Loader.GetArray<WwiseMetadataBusReference>(this, WWISE_DB_TEXT("AuxBusRefs"))),
	SwitchContainers(Loader.GetArray<WwiseMetadataSwitchContainer>(this, WWISE_DB_TEXT("SwitchContainers"))),
	ActionPostEvent(Loader.GetArray<WwiseMetadataActionPostEventEntry>(this, WWISE_DB_TEXT("ActionPostEvent"))),
	ActionSetState(Loader.GetArray<WwiseMetadataActionSetStateEntry>(this, WWISE_DB_TEXT("ActionSetState"))),
	ActionSetSwitch(Loader.GetArray<WwiseMetadataActionSetSwitchEntry>(this, WWISE_DB_TEXT("ActionSetSwitch"))),
	ActionTrigger(Loader.GetArray<WwiseMetadataActionTriggerEntry>(this, WWISE_DB_TEXT("ActionTrigger")))
{
	Loader.LogParsed(WWISE_DB_TEXT("Event"), Id, Name);
}

bool WwiseMetadataEvent::IsMandatory() const
{
	return
		(ActionPostEvent.Size() > 0)
		|| (ActionSetState.Size() > 0)
		|| (ActionSetSwitch.Size() > 0)
		|| (ActionTrigger.Size() > 0)
		|| (ActionSetFX.Size() > 0)
		|| (AuxBusRefs.Size() > 0)
		|| (ExternalSourceRefs.Size() > 0)
		|| (MediaRefs.Size() > 0)
		|| (PluginRefs && (
			(PluginRefs->Custom.Size() > 0)
			|| (PluginRefs->ShareSets.Size() > 0)
			|| (PluginRefs->AudioDevices.Size() > 0)))
		|| (SwitchContainers.Size() == 0);
}