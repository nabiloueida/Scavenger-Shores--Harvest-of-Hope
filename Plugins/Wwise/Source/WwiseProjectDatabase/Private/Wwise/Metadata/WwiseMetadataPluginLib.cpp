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

#include "Wwise/Metadata/WwiseMetadataPluginLib.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataPluginLibAttributes::WwiseMetadataPluginLibAttributes(WwiseMetadataLoader& Loader) :
	LibName(Loader.GetString(this, WWISE_DB_TEXT("LibName"))),
	LibId(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("LibId"))),
	Type(TypeFromString(Loader.GetString(this, WWISE_DB_TEXT("Type")))),
	DLL(Loader.GetString(this, WWISE_DB_TEXT("DLL"), WwiseRequiredMetadata::Optional)),
	StaticLib(Loader.GetString(this, WWISE_DB_TEXT("StaticLib"), WwiseRequiredMetadata::Optional))
{
	Loader.LogParsed(WWISE_DB_TEXT("PluginLibAttributes"), LibId, LibName);
}

WwiseMetadataPluginLibType WwiseMetadataPluginLibAttributes::TypeFromString(const WwiseDBString& TypeString)
{
	if (TypeString == WWISE_DB_TEXT("Effect"))
	{
		return WwiseMetadataPluginLibType::Effect;
	}
	else if (TypeString == WWISE_DB_TEXT("Source"))
	{
		return WwiseMetadataPluginLibType::Source;
	}
	else if (TypeString == WWISE_DB_TEXT("AudioDevice"))
	{
		return WwiseMetadataPluginLibType::AudioDevice;
	}
	else if (TypeString == WWISE_DB_TEXT("Metadata"))
	{
		return WwiseMetadataPluginLibType::Metadata;
	}
	else
	{
		WWISE_DB_LOG(Warning, "WwiseMetadataPluginLibAttributes: Unknown Type: %s", *TypeString);
		return WwiseMetadataPluginLibType::Unknown;
	}
}

WwiseMetadataPluginLib::WwiseMetadataPluginLib(WwiseMetadataLoader& Loader) :
	WwiseMetadataPluginLibAttributes(Loader)
{
	Loader.LogParsed(WWISE_DB_TEXT("PluginLib"), LibId, LibName);
}
