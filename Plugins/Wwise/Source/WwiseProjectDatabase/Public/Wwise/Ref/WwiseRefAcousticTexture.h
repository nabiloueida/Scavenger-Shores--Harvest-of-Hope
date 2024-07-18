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

#include "Wwise/Ref/WwiseRefSoundBank.h"

class WwiseRefAcousticTexture : public WwiseRefSoundBank
{
public:
	static const WwiseDBString NAME;
	static constexpr WwiseRefType TYPE = WwiseRefType::AcousticTexture;
	struct FGlobalIdsMap;

	WwiseRefIndexType AcousticTextureIndex;

	WwiseRefAcousticTexture() {}
	WwiseRefAcousticTexture(const WwiseMetadataSharedRootFileConstPtr& InRootMediaRef, const WwiseDBString& InJsonFilePath,
		WwiseRefIndexType InSoundBankIndex, WwiseDBShortId InLanguageId,
		WwiseRefIndexType InAcousticTextureIndex) :
		WwiseRefSoundBank(InRootMediaRef, InJsonFilePath, InSoundBankIndex, InLanguageId),
		AcousticTextureIndex(InAcousticTextureIndex)
	{}
	const WwiseMetadataAcousticTexture* GetAcousticTexture() const;
	
	WwiseDBShortId AcousticTextureId() const;
	WwiseDBGuid AcousticTextureGuid() const;
	WwiseDBString AcousticTextureName() const;
	WwiseDBString AcousticTextureObjectPath() const;

	WwiseDBShortId Hash() const override;
	WwiseRefType Type() const override { return TYPE; }
	bool operator==(const WwiseRefAcousticTexture& Rhs) const
	{
		return WwiseRefSoundBank::operator ==(Rhs)
			&& AcousticTextureIndex == Rhs.AcousticTextureIndex;
	}
	bool operator!=(const WwiseRefAcousticTexture& Rhs) const { return !operator==(Rhs); }
};

struct WwiseRefAcousticTexture::FGlobalIdsMap
{
	WwiseAcousticTextureGlobalIdsMap GlobalIdsMap;

	FGlobalIdsMap() {}
	FGlobalIdsMap(const FGlobalIdsMap& Rhs) :
		GlobalIdsMap(Rhs.GlobalIdsMap)
	{}
	FGlobalIdsMap(FGlobalIdsMap&& Rhs) :
		GlobalIdsMap(std::move(Rhs.GlobalIdsMap))
	{}
};
