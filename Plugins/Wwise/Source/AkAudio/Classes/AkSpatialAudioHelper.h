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

#include "Engine/EngineTypes.h"

#if WITH_EDITOR
#include "Editor/EditorEngine.h"
#endif

#include "WwiseDefines.h"
#include "WwiseUnrealDefines.h"
#if UE_5_1_OR_LATER
#include "Engine/HitResult.h"
#endif

namespace AkSpatialAudioHelper
{
	bool IsAkSpatialAudioActorClass(const AActor* Actor);

#if WITH_EDITOR
#if UE_5_0_OR_LATER
	FCoreUObjectDelegates::FOnObjectsReplaced* GetObjectReplacedEvent();
#else
	UEditorEngine::FObjectsReplacedEvent* GetObjectReplacedEvent();
#endif // UE_5_0_OR_LATER
#endif // WITH_EDITOR

}
