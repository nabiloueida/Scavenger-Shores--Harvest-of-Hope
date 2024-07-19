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

#include "Wwise/Metadata/WwiseMetadataMedia.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataMediaReference::WwiseMetadataMediaReference(WwiseMetadataLoader& Loader) :
	Id(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("Id")))
{
	Loader.LogParsed(WWISE_DB_TEXT("MediaReference"), Id);
}

WwiseMetadataMediaAttributes::WwiseMetadataMediaAttributes(WwiseMetadataLoader& Loader) :
	WwiseMetadataMediaReference(Loader),
	Language(Loader.GetString(this, WWISE_DB_TEXT("Language"))),
	bStreaming(Loader.GetBool(this, WWISE_DB_TEXT("Streaming"))),
	Location(LocationFromString(Loader.GetString(this, WWISE_DB_TEXT("Location")))),
	bUsingReferenceLanguage(Loader.GetBool(this, WWISE_DB_TEXT("UsingReferenceLanguage"), WwiseRequiredMetadata::Optional)),
	Align(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("Align"), WwiseRequiredMetadata::Optional)),
	bDeviceMemory(Loader.GetBool(this, WWISE_DB_TEXT("DeviceMemory"), WwiseRequiredMetadata::Optional))
{
	Loader.LogParsed(WWISE_DB_TEXT("MediaAttributes"), Id);
}

WwiseMetadataMediaLocation WwiseMetadataMediaAttributes::LocationFromString(const WwiseDBString& LocationString)
{
	if (LocationString == WWISE_DB_TEXT("Memory"))
	{
		return WwiseMetadataMediaLocation::Memory;
	}
	else if (LocationString == WWISE_DB_TEXT("Loose"))
	{
		return WwiseMetadataMediaLocation::Loose;
	}
	else if (LocationString == WWISE_DB_TEXT("OtherBank"))
	{
		return WwiseMetadataMediaLocation::OtherBank;
	}
	else
	{
		WWISE_DB_LOG(Warning, "WwiseMetadataMediaAttributes: Unknown Location: %s", *LocationString);
		return WwiseMetadataMediaLocation::Unknown;
	}
}

WwiseMetadataMedia::WwiseMetadataMedia(WwiseMetadataLoader& Loader) :
	WwiseMetadataMediaAttributes(Loader),
	ShortName(Loader.GetString(this, WWISE_DB_TEXT("ShortName"))),
	Path(Loader.GetString(this, WWISE_DB_TEXT("Path"), WwiseRequiredMetadata::Optional)),
	CachePath(Loader.GetString(this, WWISE_DB_TEXT("CachePath"), WwiseRequiredMetadata::Optional)),
	PrefetchSize(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("PrefetchSize"), WwiseRequiredMetadata::Optional))
{
	if (Path.IsEmpty() && Location == WwiseMetadataMediaLocation::Loose)
	{
		Loader.Fail(WWISE_DB_TEXT("!Path+Location=Loose"));
	}
	else if (Path.IsEmpty() && Location == WwiseMetadataMediaLocation::Memory && bStreaming)
	{
		Loader.Fail(WWISE_DB_TEXT("!Path+Streaming"));
	}
	else if (!Path.IsEmpty() && Location == WwiseMetadataMediaLocation::Memory && !bStreaming)
	{
		Loader.Fail(WWISE_DB_TEXT("Path+Memory"));
	}
	Loader.LogParsed(WWISE_DB_TEXT("Media"), Id);
}
