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

#include "Wwise/Stats/AkAudioMemory.h"
#include "WwiseDefines.h"

LLM_DEFINE_TAG(Audio_Wwise_SoundEngine, TEXT("SoundEngine Reserved"));

#if WWISE_2024_1_OR_LATER
DEFINE_STAT(STAT_WwiseMemorySoundEnginePrimary);
DEFINE_STAT(STAT_WwiseMemorySoundEngineMedia);
DEFINE_STAT(STAT_WwiseMemorySoundEngineProfiler);
#else
DEFINE_STAT(STAT_WwiseMemorySoundEngineVM);
#endif

#if AK_SUPPORT_DEVICE_MEMORY
DEFINE_STAT(STAT_WwiseMemorySoundEngineDevice);
#endif
