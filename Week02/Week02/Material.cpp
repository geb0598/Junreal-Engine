#include "pch.h"
#include "Material.h"
#include "Shader.h"
#include"Texture.h"

void UMaterial::SetShader( UShader* ShaderResource) {
    
	Shader = ShaderResource;
}

UShader* UMaterial::GetShader()
{
	return Shader;
}

void UMaterial::SetTexture(UTexture* TextureResource)
{
	Texture = TextureResource;
}

UTexture* UMaterial::GetTexture()
{
	return Texture;
}
