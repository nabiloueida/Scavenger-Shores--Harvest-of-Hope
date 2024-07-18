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

#include "Wwise/API_2023_1/WwiseStreamMgrAPI_2023_1.h"
#include "Wwise/Stats/SoundEngine.h"

AK::IAkStreamMgr* FWwiseStreamMgrAPI_2023_1::GetAkStreamMgrInstance()
{
	IWwiseSoundEngineModule::ForceLoadModule();
	return AK::IAkStreamMgr::Get();
}

AK::IAkStreamMgr* FWwiseStreamMgrAPI_2023_1::Create(
	const AkStreamMgrSettings& in_settings
)
{
	SCOPED_WWISESOUNDENGINE_EVENT(TEXT("AK::StreamMgr::Create"));
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::Create(in_settings);
}

void FWwiseStreamMgrAPI_2023_1::GetDefaultSettings(
	AkStreamMgrSettings& out_settings
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	AK::StreamMgr::GetDefaultSettings(out_settings);
}

AK::StreamMgr::IAkFileLocationResolver* FWwiseStreamMgrAPI_2023_1::GetFileLocationResolver()
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::GetFileLocationResolver();
}

void FWwiseStreamMgrAPI_2023_1::SetFileLocationResolver(
	AK::StreamMgr::IAkFileLocationResolver* in_pFileLocationResolver
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	AK::StreamMgr::SetFileLocationResolver(in_pFileLocationResolver);
}

AKRESULT FWwiseStreamMgrAPI_2023_1::CreateDevice(
	const AkDeviceSettings& in_settings,		///< Device settings.
	AK::StreamMgr::IAkLowLevelIOHook* in_pLowLevelHook,	///< Associated low-level I/O hook. Pass either a IAkLowLevelIOHook interface, consistent with the type of the scheduler.
	AkDeviceID& out_idDevice		///< Assigned unique device id to use in all other functions of this interface.
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::CreateDevice(in_settings, in_pLowLevelHook, out_idDevice);
}

AKRESULT FWwiseStreamMgrAPI_2023_1::DestroyDevice(
	AkDeviceID					in_deviceID
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::DestroyDevice(in_deviceID);
}

AKRESULT FWwiseStreamMgrAPI_2023_1::PerformIO()
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::PerformIO();
}

void FWwiseStreamMgrAPI_2023_1::GetDefaultDeviceSettings(
	AkDeviceSettings& out_settings
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	AK::StreamMgr::GetDefaultDeviceSettings(out_settings);
}

AKRESULT FWwiseStreamMgrAPI_2023_1::SetCurrentLanguage(
	const AkOSChar* in_pszLanguageName
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::SetCurrentLanguage(in_pszLanguageName);
}

const AkOSChar* FWwiseStreamMgrAPI_2023_1::GetCurrentLanguage()
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::GetCurrentLanguage();
}

AKRESULT FWwiseStreamMgrAPI_2023_1::AddLanguageChangeObserver(
	AK::StreamMgr::AkLanguageChangeHandler in_handler,
	void* in_pCookie
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	return AK::StreamMgr::AddLanguageChangeObserver(in_handler, in_pCookie);
}

void FWwiseStreamMgrAPI_2023_1::RemoveLanguageChangeObserver(
	void* in_pCookie
)
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	AK::StreamMgr::RemoveLanguageChangeObserver(in_pCookie);
}

void FWwiseStreamMgrAPI_2023_1::FlushAllCaches()
{
	SCOPE_CYCLE_COUNTER(STAT_WwiseSoundEngineAPI);
	AK::StreamMgr::FlushAllCaches();
}

