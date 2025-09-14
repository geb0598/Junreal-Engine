#pragma once
#include <d3d11.h>
#include "Object.h"

class UShader;
class UTexture;
class UMaterial : public UObject
{
	DECLARE_CLASS(UMaterial, UObject)
public:
    UMaterial() {};

protected:
    ~UMaterial() override {};

public:
    // ───────────────
// Resource Accessors
// ───────────────
    void SetShader(UShader* ShaderResource);
    UShader* GetShader();

    void SetTexture(UTexture* TextureResource);
    UTexture* GetTexture();

private:

	
	UShader* Shader = nullptr;
	UTexture* Texture= nullptr;
};

