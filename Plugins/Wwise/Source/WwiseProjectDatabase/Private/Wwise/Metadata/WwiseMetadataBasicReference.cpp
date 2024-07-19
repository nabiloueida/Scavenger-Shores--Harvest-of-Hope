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

#include "Wwise/Metadata/WwiseMetadataBasicReference.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataBasicReference::WwiseMetadataBasicReference()
{
	WWISE_DB_LOG(Error, "Using default WwiseMetadataBasicReference");
}

WwiseMetadataBasicReference::WwiseMetadataBasicReference(WwiseMetadataLoader& Loader) :
	Id(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("Id"))),
	Name(Loader.GetString(this, WWISE_DB_TEXT("Name"))),
	ObjectPath(Loader.GetString(this, WWISE_DB_TEXT("ObjectPath"))),
	GUID(Loader.GetGuid(this, WWISE_DB_TEXT("GUID")))
{
	Loader.LogParsed(WWISE_DB_TEXT("BasicReference"), Id, Name);
}
