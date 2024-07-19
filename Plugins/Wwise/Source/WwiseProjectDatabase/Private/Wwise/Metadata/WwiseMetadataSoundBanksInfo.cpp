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

#include "Wwise/Metadata/WwiseMetadataSoundBanksInfo.h"
#include "Wwise/Metadata/WwiseMetadataRootPaths.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataSoundBanksInfoAttributes::WwiseMetadataSoundBanksInfoAttributes(WwiseMetadataLoader& Loader):
	Platform(Loader.GetString(this, WWISE_DB_TEXT("Platform"))),
	BasePlatform(Loader.GetString(this, WWISE_DB_TEXT("BasePlatform"))),
	SchemaVersion(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("SchemaVersion"))),
	SoundBankVersion(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("SoundBankVersion")))
{
	Loader.LogParsed(WWISE_DB_TEXT("SoundBanksInfoAttributes"));
}

WwiseMetadataSoundBanksInfo::WwiseMetadataSoundBanksInfo(WwiseMetadataLoader& Loader) :
	WwiseMetadataSoundBanksInfoAttributes(Loader),
	RootPaths(Loader.GetObjectPtr<WwiseMetadataRootPaths>(this, WWISE_DB_TEXT("RootPaths"))),
	DialogueEvents(Loader.GetArray<WwiseMetadataDialogueEvent>(this, WWISE_DB_TEXT("DialogueEvents"))),
	SoundBanks(Loader.GetArray<WwiseMetadataSoundBank>(this, WWISE_DB_TEXT("SoundBanks"))),
	FileHash(Loader.GetGuid(this, WWISE_DB_TEXT("FileHash")))
{
	Loader.LogParsed(WWISE_DB_TEXT("SoundBanksInfo"));
}

WwiseMetadataSoundBanksInfo::~WwiseMetadataSoundBanksInfo()
{
	if (RootPaths)
	{
		delete RootPaths;
		RootPaths = nullptr;
	}
}
