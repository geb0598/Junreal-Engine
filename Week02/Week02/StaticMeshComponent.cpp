#include "StaticMeshComponent.h"
#include "ResourceManager.h"

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
