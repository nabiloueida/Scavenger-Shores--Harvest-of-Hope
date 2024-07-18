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

#include "Wwise/AdapterTypes/WwiseDataTypesAdapter.h"
#include "Wwise/Metadata/WwiseMetadataLoadable.h"
#include "Wwise/WwiseDatabaseIdentifiers.h"

struct WwiseMetadataExternalSourceReference : public WwiseMetadataLoadable
{
	WwiseDBShortId Cookie;

	WwiseMetadataExternalSourceReference(WwiseMetadataLoader& Loader);
};

inline WwiseDBShortId GetTypeHash(const WwiseMetadataExternalSourceReference& Ref)
{
	return GetTypeHash(Ref.Cookie);
}
inline bool operator==(const WwiseMetadataExternalSourceReference& Lhs, const WwiseMetadataExternalSourceReference& Rhs)
{
	return Lhs.Cookie == Rhs.Cookie;
}
inline bool operator<(const WwiseMetadataExternalSourceReference& Lhs, const WwiseMetadataExternalSourceReference& Rhs)
{
	return Lhs.Cookie < Rhs.Cookie;
}

struct WwiseMetadataExternalSource : public WwiseMetadataExternalSourceReference
{
	WwiseDBString Name;
	WwiseDBString ObjectPath;
	WwiseDBGuid GUID;

	WwiseMetadataExternalSource(WwiseMetadataLoader& Loader);
};
