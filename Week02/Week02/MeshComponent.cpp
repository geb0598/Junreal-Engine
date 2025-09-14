#include "pch.h"
#include "MeshComponent.h"

UMeshComponent::UMeshComponent()
    : Material(nullptr)
{
}

UMeshComponent::~UMeshComponent()
{
    Material = nullptr;
}
