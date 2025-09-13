#pragma once
#include "ResourceBase.h"

class UTexture : public UResourceBase
{
public:
	DECLARE_CLASS(UTexture, UResourceBase)

	void Load(const FString& FilePath);
};