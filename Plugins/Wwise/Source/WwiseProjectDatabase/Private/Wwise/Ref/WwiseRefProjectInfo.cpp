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

#include "Wwise/Ref/WwiseRefProjectInfo.h"

#include "Wwise/Metadata/WwiseMetadataRootFile.h"

const WwiseDBString WwiseRefProjectInfo::NAME = WWISE_DB_TEXT("ProjectInfo");

const WwiseMetadataProjectInfo* WwiseRefProjectInfo::GetProjectInfo() const
{
	const auto* RootFile = GetRootFile();
	if (!RootFile) [[unlikely]]
	{
		return nullptr;
	}
	WWISE_DB_CLOG(!RootFile->ProjectInfo, Error, "Could not get ProjectInfo");
	return RootFile->ProjectInfo;
}
