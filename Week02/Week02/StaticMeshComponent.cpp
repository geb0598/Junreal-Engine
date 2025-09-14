#include "pch.h"
#include "StaticMeshComponent.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "D3D11RHI.h"
#include  "Renderer.h"

UStaticMeshComponent::UStaticMeshComponent()
{
    StaticMesh = nullptr;
}

UStaticMeshComponent::~UStaticMeshComponent()
{

}

void UStaticMeshComponent::SetStaticMesh(const FString& FilePath)
{
    StaticMesh = UResourceManager::GetInstance().GetOrCreateStaticMesh(FilePath);
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

void UStaticMeshComponent::Render(URenderer* Renderer, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
{
    Renderer->UpdateConstantBuffer(GetWorldMatrix(), ViewMatrix, ProjectionMatrix);
    Renderer->PrepareShader(GetShader());
    Renderer->DrawIndexedPrimitiveComponent(GetMesh(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

