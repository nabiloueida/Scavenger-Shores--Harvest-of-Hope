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

#include "Wwise/API_2024_1/Platforms/XboxOneGDKAPI_2024_1.h"
#include "Wwise/Stats/SoundEngine.h"

#if defined(PLATFORM_XBOXONEGDK) && PLATFORM_XBOXONEGDK
AkUInt32 FWwisePlatformAPI_2024_1_XboxOneGDK::GetDeviceID(IMMDevice* in_pDevice)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::GetDeviceID(in_pDevice);
}

AkUInt32 FWwisePlatformAPI_2024_1_XboxOneGDK::GetDeviceIDFromName(wchar_t* in_szToken)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::GetDeviceIDFromName(in_szToken);
}

AkUInt32 FWwisePlatformAPI_2024_1_XboxOneGDK::GetGameInputDeviceID(const IGameInputDevice* in_pGameInputDevice)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::SoundEngine::GetGameInputDeviceID(in_pGameInputDevice);
}
#endif
