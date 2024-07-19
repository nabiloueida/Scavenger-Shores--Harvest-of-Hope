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

#include "Wwise/API_2022_1/Platforms/IOS/IOSAPI_2022_1.h"
#include "Wwise/Stats/SoundEngine.h"

#if defined(PLATFORM_IOS) && PLATFORM_IOS && !(defined(PLATFORM_TVOS) && PLATFORM_TVOS)

#if !defined(__has_include) || __has_include("Generated/AkiOSPlugins.h")
#include "Generated/AkiOSPlugins.h"
#endif

void FWwisePlatformAPI_2022_1_IOS::ChangeAudioSessionProperties(
	const AkAudioSessionProperties &in_properties
	)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	AK::SoundEngine::iOS::ChangeAudioSessionProperties(in_properties);
}

AkDeviceID FWwisePlatformAPI_2022_1_IOS::GetDeviceIDFromPlayerIndex(int playerIndex)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK_INVALID_DEVICE_ID;
}
#endif
