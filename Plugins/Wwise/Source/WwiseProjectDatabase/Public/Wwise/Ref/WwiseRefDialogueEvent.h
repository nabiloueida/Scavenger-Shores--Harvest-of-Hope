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

class WwiseRefDialogueEvent : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::DialogueEvent;
	struct FGlobalIdsMap;

	WwiseRefIndexType DialogueEventIndex;

	WwiseRefDialogueEvent() {}
	WwiseRefDialogueEvent(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InDialogueEventIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		DialogueEventIndex(InDialogueEventIndex)
	{}
	const WwiseMetadataDialogueEvent* GetDialogueEvent() const;
	WwiseDialogueArgumentIdsMap GetDialogueArguments(const WwiseDialogueArgumentGlobalIdsMap& GlobalMap) const;

	WwiseDBShortId DialogueEventId() const;
	WwiseDBGuid DialogueEventGuid() const;
	WwiseDBString DialogueEventName() const;
	WwiseDBString DialogueEventObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefDialogueEvent& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& DialogueEventIndex == Rhs.DialogueEventIndex;
	}
	bool operator!=(const WwiseRefDialogueEvent& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefDialogueEvent::FGlobalIdsMap
{
	WwiseDialogueEventGlobalIdsMap GlobalIdsMap;
};
