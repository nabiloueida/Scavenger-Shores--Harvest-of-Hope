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

#include "Wwise/Metadata/WwiseMetadataLoadable.h"
#include "Wwise/Ref/WwiseRefCollections.h"

struct WwiseMetadataBasicReference : public WwiseMetadataLoadable
{
	WwiseDBShortId Id;
	WwiseDBString Name;
	WwiseDBString ObjectPath;
	WwiseDBGuid GUID;

	WwiseMetadataBasicReference();
	WwiseMetadataBasicReference(WwiseDBShortId InId, WwiseDBString&& InName,
								 WwiseDBString&& InObjectPath, WwiseDBGuid&& InGUID) :
		Id(std::move(InId)),
		Name(std::move(InName)),
		ObjectPath(std::move(InObjectPath)),
		GUID(std::move(InGUID))
	{}
	WwiseMetadataBasicReference(WwiseDBShortId InId, const WwiseDBString& InName,
								 const WwiseDBString& InObjectPath, const WwiseDBGuid& InGUID) :
		Id(InId),
		Name(InName),
		ObjectPath(InObjectPath),
		GUID(InGUID)
	{}
	WwiseMetadataBasicReference(WwiseMetadataLoader& Loader);

	bool operator<(const WwiseMetadataBasicReference& Rhs)
	{
		return Id < Rhs.Id;
	}

	bool operator==(const WwiseMetadataBasicReference& Rhs)
	{
		return Id == Rhs.Id;
	}
};

inline WwiseDBShortId GetTypeHash(const WwiseMetadataBasicReference& Ref)
{
	return GetTypeHash(Ref.Id);
}
inline bool operator==(const WwiseMetadataBasicReference& Lhs, const WwiseMetadataBasicReference& Rhs)
{
	return Lhs.Id == Rhs.Id;
}
inline bool operator<(const WwiseMetadataBasicReference& Lhs, const WwiseMetadataBasicReference& Rhs)
{
	return Lhs.Id < Rhs.Id;
}
