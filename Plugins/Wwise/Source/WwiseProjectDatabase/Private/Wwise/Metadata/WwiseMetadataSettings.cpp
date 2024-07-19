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

#include "Wwise/Metadata/WwiseMetadataSettings.h"
#include "Wwise/Metadata/WwiseMetadataLoader.h"

WwiseMetadataSettings::WwiseMetadataSettings() :
	bAutoSoundBankDefinition(false),
	bCopyLooseStreamedMediaFiles(false),
	bSubFoldersForGeneratedFiles(false),
	bRemoveUnusedGeneratedFiles(false),
	bSourceControlGeneratedFiles(false),
	bGenerateHeaderFile(false),
	bGenerateContentTxtFile(false),
	bGenerateMetadataXML(false),
	bGenerateMetadataJSON(false),
	bGenerateAllBanksMetadata(false),
	bGeneratePerBankMetadata(false),
	bUseSoundBankNames(false),
	bAllowExceedingMaxSize(false),
	bMaxAttenuationInfo(false),
	bEstimatedDurationInfo(false),
	bPrintObjectGuid(false),
	bPrintObjectPath(false)
{
	WWISE_DB_LOG(Error, "Using default Settings");
}

WwiseMetadataSettings::WwiseMetadataSettings(WwiseMetadataLoader& Loader) :
	bAutoSoundBankDefinition(Loader.GetBool(this, WWISE_DB_TEXT("AutoSoundBankDefinition"))),
	bCopyLooseStreamedMediaFiles(Loader.GetBool(this, WWISE_DB_TEXT("CopyLooseStreamedMediaFiles"))),
	bSubFoldersForGeneratedFiles(Loader.GetBool(this, WWISE_DB_TEXT("SubFoldersForGeneratedFiles"))),
	bRemoveUnusedGeneratedFiles(Loader.GetBool(this, WWISE_DB_TEXT("RemoveUnusedGeneratedFiles"))),
	bSourceControlGeneratedFiles(Loader.GetBool(this, WWISE_DB_TEXT("SourceControlGeneratedFiles"))),
	bGenerateHeaderFile(Loader.GetBool(this, WWISE_DB_TEXT("GenerateHeaderFile"))),
	bGenerateContentTxtFile(Loader.GetBool(this, WWISE_DB_TEXT("GenerateContentTxtFile"))),
	bGenerateMetadataXML(Loader.GetBool(this, WWISE_DB_TEXT("GenerateMetadataXML"))),
	bGenerateMetadataJSON(Loader.GetBool(this, WWISE_DB_TEXT("GenerateMetadataJSON"))),
	bGenerateAllBanksMetadata(Loader.GetBool(this, WWISE_DB_TEXT("GenerateAllBanksMetadata"))),
	bGeneratePerBankMetadata(Loader.GetBool(this, WWISE_DB_TEXT("GeneratePerBankMetadata"))),
	bUseSoundBankNames(Loader.GetBool(this, WWISE_DB_TEXT("UseSoundBankNames"))),
	bAllowExceedingMaxSize(Loader.GetBool(this, WWISE_DB_TEXT("AllowExceedingMaxSize"))),
	bMaxAttenuationInfo(Loader.GetBool(this, WWISE_DB_TEXT("MaxAttenuationInfo"))),
	bEstimatedDurationInfo(Loader.GetBool(this, WWISE_DB_TEXT("EstimatedDurationInfo"))),
	bPrintObjectGuid(Loader.GetBool(this, WWISE_DB_TEXT("PrintObjectGuid"))),
	bPrintObjectPath(Loader.GetBool(this, WWISE_DB_TEXT("PrintObjectPath")))
{
	Loader.LogParsed(WWISE_DB_TEXT("Settings"));
}
