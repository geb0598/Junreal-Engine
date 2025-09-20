#include "pch.h"
#include "MeshComponent.h"
#include "StaticMesh.h"

UMeshComponent::UMeshComponent()
    
{
}

UMeshComponent::~UMeshComponent()
{
    Material = nullptr;
}

void UMeshComponent::SetMeshResource(const FString& FilePath)
{
    if (UResourceManager::GetInstance().Get<UStaticMesh>(FilePath))
    {
        MeshResource = UResourceManager::GetInstance().Get<UStaticMesh>(FilePath);
    }
    else
    {
        MeshResource = UResourceManager::GetInstance().Load<UStaticMesh>(FilePath);
    }
}