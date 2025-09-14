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





void UStaticMeshComponent::Render(URenderer* Renderer, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
{
    Renderer->UpdateConstantBuffer(GetWorldMatrix(), ViewMatrix, ProjectionMatrix);
    Renderer->PrepareShader(GetMaterial()->GetShader());
    Renderer->DrawIndexedPrimitiveComponent(GetMesh(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

}

