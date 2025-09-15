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

void UStaticMeshComponent::Render(URenderer* Renderer, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
{
    Renderer->UpdateConstantBuffer(GetWorldMatrix(), ViewMatrix, ProjectionMatrix);
    Renderer->PrepareShader(GetMaterial()->GetShader());
    Renderer->DrawIndexedPrimitiveComponent(GetMeshResource(), D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

