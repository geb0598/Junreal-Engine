#pragma once
#include <d3d11.h>
#include "Object.h"

class UResourceBase : public UObject
{
public:
	DECLARE_CLASS(UResourceBase, UObject)

	UResourceBase() = default;
	~UResourceBase() = default;

protected:
	FString FilePath;
};