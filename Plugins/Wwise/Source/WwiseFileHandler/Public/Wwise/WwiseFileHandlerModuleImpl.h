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

#include "Wwise/WwiseFileHandlerModule.h"
#include "Wwise/WwiseFileCache.h"
#include "Wwise/WwiseExternalSourceManager.h"
#include "Wwise/WwiseMediaManager.h"
#include "Wwise/WwiseSoundBankManager.h"

class WWISEFILEHANDLER_API FWwiseFileHandlerModule : public IWwiseFileHandlerModule
{
public:
	FWwiseFileHandlerModule();

	IWwiseSoundBankManager* GetSoundBankManager() override;
	IWwiseExternalSourceManager* GetExternalSourceManager() override;
	IWwiseMediaManager* GetMediaManager() override;
	FWwiseFileCache* GetFileCache() override;
	FWwiseExecutionQueue* GetBankExecutionQueue() override; 
	FWwiseIOHook* InstantiateIOHook() override;
	IWwiseSoundBankManager* InstantiateSoundBankManager() override;
	IWwiseExternalSourceManager* InstantiateExternalSourceManager() override;
	IWwiseMediaManager* InstantiateMediaManager() override;
	FWwiseFileCache* InstantiateFileCache() override;
	FWwiseExecutionQueue* InstantiateBankExecutionQueue() override; 

	void StartupModule() override;
	void ShutdownModule() override;

protected:
	FRWLock Lock;
	TUniquePtr<IWwiseExternalSourceManager> ExternalSourceManager;
	TUniquePtr<IWwiseMediaManager> MediaManager;
	TUniquePtr<IWwiseSoundBankManager> SoundBankManager;
	TUniquePtr<FWwiseFileCache> FileCache;
	TUniquePtr<FWwiseExecutionQueue> BankExecutionQueue;
};
