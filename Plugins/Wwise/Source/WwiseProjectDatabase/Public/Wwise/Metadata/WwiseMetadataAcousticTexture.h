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

#include "Wwise/Metadata/WwiseMetadataBasicReference.h"
#include "WwiseDefines.h"

struct WwiseMetadataAcousticTexture : public WwiseMetadataBasicReference
{
	float AbsorptionLow { 0 };
	float AbsorptionMidLow { 0 };
	float AbsorptionMidHigh { 0 };
	float AbsorptionHigh { 0 };
#if WWISE_2023_1_OR_LATER
	float Scattering { 0 };
#endif

	WwiseMetadataAcousticTexture(WwiseMetadataLoader& Loader);

private:
	static const WwiseDBMap<WwiseDBString, size_t> FloatProperties;
	static const WwiseDBMap<WwiseDBString, size_t> FillFloatProperties();
};

