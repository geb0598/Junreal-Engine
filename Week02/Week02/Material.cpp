#include "pch.h"
#include "Material.h"
#include "Shader.h"
#include"Texture.h"

void UMaterial::SetShader( UShader* ShaderResource) {
    
	Shader = ShaderResource;
}

void UMaterial::SetTexture(UTexture* TextureResource)
{
	Texture = TextureResource;
}