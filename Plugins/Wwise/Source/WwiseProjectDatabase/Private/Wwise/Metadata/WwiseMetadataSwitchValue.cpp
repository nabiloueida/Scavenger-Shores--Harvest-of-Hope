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

#include "Wwise/Metadata/WwiseMetadataSwitchValue.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataSwitchValueAttributes::WwiseMetadataSwitchValueAttributes()
{
}

WwiseMetadataSwitchValueAttributes::WwiseMetadataSwitchValueAttributes(WwiseMetadataLoader& Loader) :
	GroupType(GroupTypeFromString(Loader.GetString(this, WWISE_DB_TEXT("GroupType")))),
	GroupId(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("GroupId"))),
	Id(Loader.GetWwiseShortId(this, WWISE_DB_TEXT("Id"))),
	GUID(Loader.GetGuid(this, WWISE_DB_TEXT("GUID"))),
	bDefault(Loader.GetBool(this, WWISE_DB_TEXT("Default"), WwiseRequiredMetadata::Optional))
{
	Loader.LogParsed(WWISE_DB_TEXT("SwitchValueAttributes"));
}

WwiseMetadataSwitchValueGroupType WwiseMetadataSwitchValueAttributes::GroupTypeFromString(const WwiseDBString& TypeString)
{
	if (TypeString == WWISE_DB_TEXT("Switch"))
	{
		return WwiseMetadataSwitchValueGroupType::Switch;
	}
	else if (TypeString == WWISE_DB_TEXT("State"))
	{
		return WwiseMetadataSwitchValueGroupType::State;
	}
	WWISE_DB_LOG(Warning, "WwiseMetadataSwitchValueAttributes: Unknown GroupType: %s", *TypeString);
	return WwiseMetadataSwitchValueGroupType::Unknown;
}

WwiseMetadataSwitchValue::WwiseMetadataSwitchValue()
{
}

WwiseMetadataSwitchValue::WwiseMetadataSwitchValue(WwiseMetadataLoader& Loader) :
	WwiseMetadataSwitchValueAttributes(Loader)
{
	Loader.LogParsed(WWISE_DB_TEXT("SwitchValue"));
}
