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

#include "Wwise/Metadata/WwiseMetadataSwitchContainer.h"
#include "Wwise/Metadata/WwiseMetadataPluginGroup.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataSwitchContainer::WwiseMetadataSwitchContainer(WwiseMetadataLoader& Loader) :
	SwitchValue(Loader.GetObject<WwiseMetadataSwitchValue>(this, WWISE_DB_TEXT("SwitchValue"))),
	MediaRefs(Loader.GetArray<WwiseMetadataMediaReference>(this, WWISE_DB_TEXT("MediaRefs"))),
	ExternalSourceRefs(Loader.GetArray<WwiseMetadataExternalSourceReference>(this, WWISE_DB_TEXT("ExternalSourceRefs"))),
	PluginRefs(Loader.GetObjectPtr<WwiseMetadataPluginReferenceGroup>(this, WWISE_DB_TEXT("PluginRefs"))),
	Children(Loader.GetArray<WwiseMetadataSwitchContainer>(this, WWISE_DB_TEXT("Children")))
{
	Loader.LogParsed(WWISE_DB_TEXT("SwitchContainer"));
}

WwiseDBSet<WwiseMetadataMediaReference> WwiseMetadataSwitchContainer::GetAllMedia() const
{
	WwiseDBSet<WwiseMetadataMediaReference> Result(MediaRefs);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllMedia());
	}
	return Result;
}

WwiseDBSet<WwiseMetadataExternalSourceReference> WwiseMetadataSwitchContainer::GetAllExternalSources() const
{
	WwiseDBSet<WwiseMetadataExternalSourceReference> Result(ExternalSourceRefs);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllExternalSources());
	}
	return Result;
}

WwiseDBSet<WwiseMetadataPluginReference> WwiseMetadataSwitchContainer::GetAllCustomPlugins() const
{
	if (!PluginRefs)
	{
		return {};
	}
	WwiseDBSet<WwiseMetadataPluginReference> Result(PluginRefs->Custom);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllCustomPlugins());
	}
	return Result;
}

WwiseDBSet<WwiseMetadataPluginReference> WwiseMetadataSwitchContainer::GetAllPluginShareSets() const
{
	if (!PluginRefs)
	{
		return {};
	}
	WwiseDBSet<WwiseMetadataPluginReference> Result(PluginRefs->ShareSets);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllPluginShareSets());
	}
	return Result;
}

WwiseDBSet<WwiseMetadataPluginReference> WwiseMetadataSwitchContainer::GetAllAudioDevices() const
{
	if (!PluginRefs)
	{
		return {};
	}
	WwiseDBSet<WwiseMetadataPluginReference> Result(PluginRefs->AudioDevices);
	for (const auto& Child : Children)
	{
		Result.Append(Child.GetAllAudioDevices());
	}
	return Result;
}
