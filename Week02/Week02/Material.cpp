#include "pch.h"
#include "Material.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"

void UMaterial::Load(const FString& InFilePath, ID3D11Device* InDevice, EVertexLayoutType InLayoutType)
{
    // 기본 쉐이더 로드 (LayoutType에 따라)
    FString shaderName;
    if (InLayoutType == EVertexLayoutType::PositionColor)
        shaderName = "Primitive.hlsl";
    else if (InLayoutType == EVertexLayoutType::PositionBillBoard)
        shaderName = "TextBillboard.hlsl";
    else if (InLayoutType == EVertexLayoutType::PositionCollisionDebug)
        shaderName = "CollisionDebug.hlsl";
    
    if (!shaderName.empty())
    {
        Shader = UResourceManager::GetInstance().Load<UShader>(shaderName, InLayoutType);
    }
    
    // 텍스처가 지정되었다면 로드
    if (!InFilePath.empty())
    {
        Texture = UResourceManager::GetInstance().Load<UTexture>(InFilePath);
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
