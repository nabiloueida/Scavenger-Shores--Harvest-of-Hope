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

#include "Wwise/Metadata/WwiseMetadataPlatform.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataPlatformAttributes::WwiseMetadataPlatformAttributes()
{
	WWISE_DB_LOG(Error, "Using default WwiseMetadataPlatformAttributes");
}

WwiseMetadataPlatformAttributes::WwiseMetadataPlatformAttributes(WwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, WWISE_DB_TEXT("Name"))),
	BasePlatform(Loader.GetString(this, WWISE_DB_TEXT("BasePlatform"))),
	Generator(Loader.GetString(this, WWISE_DB_TEXT("Generator")))
{
	Loader.LogParsed(WWISE_DB_TEXT("PlatformAttributes"), 0, Name);
}

WwiseMetadataPlatformReference::WwiseMetadataPlatformReference(WwiseMetadataLoader& Loader) :
	Name(Loader.GetString(this, WWISE_DB_TEXT("Name"))),
	GUID(Loader.GetGuid(this, WWISE_DB_TEXT("GUID"))),
	BasePlatform(Loader.GetString(this, WWISE_DB_TEXT("BasePlatform"))),
	BasePlatformGUID(Loader.GetGuid(this, WWISE_DB_TEXT("BasePlatformGUID"))),
	Path(Loader.GetString(this, WWISE_DB_TEXT("Path")))
{
	Loader.LogParsed(WWISE_DB_TEXT("PlatformReference"), 0, Name);
}

WwiseMetadataPlatform::WwiseMetadataPlatform()
{
	WWISE_DB_LOG(Error, "Using default WwiseMetadataPlatform");
}

WwiseMetadataPlatform::WwiseMetadataPlatform(WwiseMetadataLoader& Loader) :
	WwiseMetadataPlatformAttributes(Loader)
{
	Loader.LogParsed(WWISE_DB_TEXT("Platform"), 0, Name);
}
