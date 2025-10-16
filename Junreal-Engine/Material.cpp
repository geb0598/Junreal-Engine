﻿#include "pch.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"

void UMaterial::Load(const FString& InFilePath, ID3D11Device* InDevice)
{
    // 기본 쉐이더 로드 (LayoutType에 따라)

    if (InFilePath.find(".dds") != std::string::npos)
    {
       
        /*FString shaderName = UResourceManager::GetInstance().GetProperShader(InFilePath);
        if (Shader) {
            Shader = UResourceManager::GetInstance().Load<UShader>(shaderName);
        }*/
        Texture = UResourceManager::GetInstance().Load<UTexture>(InFilePath);
    }
    else if (InFilePath.find(".hlsl") != std::string::npos)
    {
        Shader = UResourceManager::GetInstance().Load<UShader>(InFilePath);
    }
    else
    {
        throw std::runtime_error(".dds나 .hlsl만 입력해주세요. 현재 입력 파일명 : " + InFilePath);
    }
}

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
