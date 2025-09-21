#include "pch.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "Shader.h"
#include "Texture.h"
#include "ResourceManager.h"
#include "ObjManager.h"

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
    Renderer->DrawIndexedPrimitiveComponent(GetStaticMesh(), D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void UStaticMeshComponent::SetStaticMesh(const FString& PathFileName)
{
	StaticMesh = FObjManager::LoadObjStaticMesh(PathFileName);
}

