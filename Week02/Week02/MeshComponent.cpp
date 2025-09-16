#include "pch.h"
#include "MeshComponent.h"
#include"Mesh.h"

UMeshComponent::UMeshComponent()
    
{
}

UMeshComponent::~UMeshComponent()
{
    Material = nullptr;
}

void UMeshComponent::SetMeshResource(const FString& FilePath)
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