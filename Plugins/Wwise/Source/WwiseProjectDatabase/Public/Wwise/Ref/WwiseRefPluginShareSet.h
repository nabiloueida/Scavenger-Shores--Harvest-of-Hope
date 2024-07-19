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

#include "Wwise/Ref/WwiseRefSoundBank.h"

class WwiseRefPluginShareSet : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::PluginShareSet;
	struct FGlobalIdsMap;

	WwiseRefIndexType PluginShareSetIndex;

	WwiseRefPluginShareSet() {}
	WwiseRefPluginShareSet(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InPluginShareSetIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		PluginShareSetIndex(InPluginShareSetIndex)
	{}
	const WwiseMetadataPlugin* GetPlugin() const;
	WwiseMediaIdsMap GetPluginMedia(const WwiseMediaGlobalIdsMap& GlobalMap) const;
	WwiseCustomPluginIdsMap GetPluginCustomPlugins(const WwiseCustomPluginGlobalIdsMap& GlobalMap) const;
	WwisePluginShareSetIdsMap GetPluginPluginShareSets(const WwisePluginShareSetGlobalIdsMap& GlobalMap) const;
	WwiseAudioDeviceIdsMap GetPluginAudioDevices(const WwiseAudioDeviceGlobalIdsMap& GlobalMap) const;

	WwiseDBShortId PluginShareSetId() const;
	WwiseDBGuid PluginShareSetGuid() const;
	WwiseDBString PluginShareSetName() const;
	WwiseDBString PluginShareSetObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefPluginShareSet& Rhs) const
	{
		return WwiseRefSoundBank::operator==(Rhs)
			&& PluginShareSetIndex == Rhs.PluginShareSetIndex;
	}
	bool operator!=(const WwiseRefPluginShareSet& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefPluginShareSet::FGlobalIdsMap
{
	WwisePluginShareSetGlobalIdsMap GlobalIdsMap;
};
