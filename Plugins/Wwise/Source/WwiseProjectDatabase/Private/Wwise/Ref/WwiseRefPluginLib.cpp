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

#include "Wwise/Ref/WwiseRefPluginLib.h"
#include "Wwise/Metadata/WwiseMetadataPluginInfo.h"
#include "Wwise/Metadata/WwiseMetadataPluginLib.h"

const WwiseDBString WwiseRefPluginLib::NAME = WWISE_DB_TEXT("PluginLib");

const WwiseMetadataPluginLib* WwiseRefPluginLib::GetPluginLib() const
{
	const auto* PluginInfo = GetPluginInfo();
	if (!PluginInfo) [[unlikely]]
	{
		return nullptr;
	}
	const auto& PluginLibs = PluginInfo->PluginLibs;
	if (PluginLibs.IsValidIndex(PluginLibIndex))
	{
		return &PluginLibs[PluginLibIndex];
	}
	else
	{
		WWISE_DB_LOG(Error, "Could not get PluginLib index #%zu", PluginLibIndex);
		return nullptr;
	}
}

WwiseDBShortId WwiseRefPluginLib::PluginLibId() const
{
	const auto* PluginLib = GetPluginLib();
	if (!PluginLib) [[unlikely]]
	{
		return 0;
	}
	return PluginLib->LibId;
}

WwiseDBString WwiseRefPluginLib::PluginLibName() const
{
	const auto* PluginLib = GetPluginLib();
	if (!PluginLib) [[unlikely]]
	{
		return {};
	}
	return PluginLib->LibName;
}

WwiseDBShortId WwiseRefPluginLib::Hash() const
{
	auto Result = WwiseRefPluginInfo::Hash();
	Result = WwiseDBHashCombine(Result, GetTypeHash(PluginLibIndex));
	return Result;
}
