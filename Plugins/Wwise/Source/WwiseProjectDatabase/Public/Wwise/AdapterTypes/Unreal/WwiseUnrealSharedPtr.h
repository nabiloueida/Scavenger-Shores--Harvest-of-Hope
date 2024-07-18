﻿/*******************************************************************************
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

#include "Wwise/AdapterTypes/WwiseSharedPtr.h"

#include "Templates/SharedPointer.h"

template<typename Type, typename UnrealSharedPtr = TSharedPtr<Type, ESPMode::ThreadSafe>>
class WwiseUnrealSharedPtr : public IWwiseSharedPtr<UnrealSharedPtr>
{
public:
	WwiseUnrealSharedPtr() {}

	~WwiseUnrealSharedPtr()
	{
		if(this->SharedPtr)
		{
			this->SharedPtr.Reset();
		}
	}

	WwiseUnrealSharedPtr(UnrealSharedPtr InSharedPtr) : IWwiseSharedPtr<UnrealSharedPtr>(InSharedPtr){}

	WwiseUnrealSharedPtr(const WwiseUnrealSharedPtr<Type>& InSharedPtr) : IWwiseSharedPtr<UnrealSharedPtr>(InSharedPtr.SharedPtr){}

	WwiseUnrealSharedPtr(Type* InPtr)
	{
		this->SharedPtr = MakeShareable(InPtr);
	}

	WwiseUnrealSharedPtr(const Type& InPtr)
	{
		this->SharedPtr = MakeShareable(new Type(InPtr));
	}
	
	Type* Get() const
	{
		return this->SharedPtr.Get();
	}

	bool IsValid() const
	{
		return this->SharedPtr.IsValid();
	}

	template<typename... ArgsType>
	void Make_Shared(ArgsType&&... Args)
	{
		this->SharedPtr = MakeShared<Type>(Args...);
	}

	bool operator!() const
	{
		return !this->SharedPtr;
	}

	bool operator!=(Type* Ptr) const
	{
		return this->SharedPtr != Ptr;
	}

	Type operator*() const
	{
		return *this->SharedPtr;
	}
	
	Type* operator->() const
	{
		return this->SharedPtr.Get();
	}

	bool operator==(std::nullptr_t Null) const
	{
		return this->SharedPtr == Null;
	}

	bool operator!=(std::nullptr_t Null) const
	{
		return this->SharedPtr != Null;
	}

	operator bool() const
	{
		return this->SharedPtr != nullptr;
	}
};