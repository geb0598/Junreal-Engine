#pragma once
#include <d3d11.h>
#include "Object.h"
class UMaterial : public UObject
{
public:
    UMaterial() {};

protected:
    ~UMaterial() override {};

public:

private:

	
	UShader* ShaderResource = nullptr;
	UTexture* TextureResource = nullptr;
};

