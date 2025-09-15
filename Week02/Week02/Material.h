#pragma once
#include "ResourceBase.h"
#include "Enums.h"

class UShader;
class UTexture;
class UMaterial : public UResourceBase
{
	DECLARE_CLASS(UMaterial, UResourceBase)
public:
    UMaterial() = default;
    void Load(const FString& InFilePath, ID3D11Device* InDevice, EVertexLayoutType InLayoutType);

protected:
    ~UMaterial() override = default;

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

