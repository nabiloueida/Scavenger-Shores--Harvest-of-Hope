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

#include "Wwise/Metadata/WwiseMetadataBus.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"
#include "Wwise/Metadata/WwiseMetadataPluginGroup.h"

WwiseMetadataBusReference::WwiseMetadataBusReference(WwiseMetadataLoader& Loader) :
	WwiseMetadataBasicReference(Loader)
{
	Loader.LogParsed(WWISE_DB_TEXT("BusReference"), Id, Name);
}

WwiseMetadataBus::WwiseMetadataBus(WwiseMetadataLoader& Loader) :
	WwiseMetadataBusReference(Loader),
	PluginRefs(Loader.GetObjectPtr<WwiseMetadataPluginReferenceGroup>(this, WWISE_DB_TEXT("PluginRefs"))),
	AuxBusRefs(Loader.GetArray<WwiseMetadataBusReference>(this, WWISE_DB_TEXT("AuxBusRefs"))),
	MaxAttenuation(Loader.GetFloat(this, WWISE_DB_TEXT("MaxAttenuation"), WwiseRequiredMetadata::Optional))
{
	Loader.LogParsed(WWISE_DB_TEXT("Bus"), Id, Name);
}
