#include "pch.h"
#include "StaticMeshComponent.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"

UStaticMeshComponent::UStaticMeshComponent()
{

}

UStaticMeshComponent::~UStaticMeshComponent()
{

}

void UStaticMeshComponent::SetMesh(const FString& FilePath)
{
    if (UResourceManager::GetInstance().Get<UMesh>(FilePath))
    {
        MeshResource = UResourceManager::GetInstance().Get<UMesh>(FilePath);
    }
    else
    {
        MeshResource = UResourceManager::GetInstance().Load<UMesh>(FilePath);
    }
}

void UStaticMeshComponent::SetShader(const FString& FilePath, EVertexLayoutType layoutType)
{
    if (UResourceManager::GetInstance().Get<UShader>(FilePath))
    {
        ShaderResource = UResourceManager::GetInstance().Get<UShader>(FilePath);
    }
    else
    {
        ShaderResource = UResourceManager::GetInstance().Load<UShader>(FilePath, layoutType);
    }
}

void UStaticMeshComponent::SetTexture(const FString& FilePath)
{
    if (UResourceManager::GetInstance().Get<UTexture>(FilePath))
    {
        TextureResource = UResourceManager::GetInstance().Get<UTexture>(FilePath);
    }
    else
    {
        TextureResource = UResourceManager::GetInstance().Load<UTexture>(FilePath);
    }
}
