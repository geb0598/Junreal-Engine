#include"pch.h"
#include "BoundingBoxComponent.h"
#include "Vector.h"

UBoundingBoxComponent::UBoundingBoxComponent()
    : LocalMin(FVector{}), LocalMax(FVector{})
{
}

void UBoundingBoxComponent::SetFromVertices(const std::vector<FVector>& Verts)
{
    if (Verts.empty()) return;

    LocalMin = LocalMax = Verts[0];
    for (auto& v : Verts)
    {
        LocalMin = LocalMin.ComponentMin(v);
        LocalMax = LocalMax.ComponentMax(v);
    }
}

FBox UBoundingBoxComponent::GetWorldBox() const
{
    auto corners = GetLocalCorners();

    FVector MinW = GetWorldTransform().TransformPosition(corners[0]);
    FVector MaxW = MinW;

    for (auto& c : corners)
    {
        FVector wc = GetWorldTransform().TransformPosition(c);
        MinW = MinW.ComponentMin(wc);
        MaxW = MaxW.ComponentMax(wc);
    }//MinW, MaxW
    return FBox();
}

FVector UBoundingBoxComponent::GetExtent() const
{
    return (LocalMax - LocalMin) * 0.5f;
}

std::vector<FVector> UBoundingBoxComponent::GetLocalCorners() const
{
    return {
        {LocalMin.X, LocalMin.Y, LocalMin.Z},
        {LocalMax.X, LocalMin.Y, LocalMin.Z},
        {LocalMin.X, LocalMax.Y, LocalMin.Z},
        {LocalMax.X, LocalMax.Y, LocalMin.Z},
        {LocalMin.X, LocalMin.Y, LocalMax.Z},
        {LocalMax.X, LocalMin.Y, LocalMax.Z},
        {LocalMin.X, LocalMax.Y, LocalMax.Z},
        {LocalMax.X, LocalMax.Y, LocalMax.Z}
    };
}

void UBoundingBoxComponent::Render(URenderer* Renderer, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
{
    //Renderer->UpdateConstantBuffer(GetWorldMatrix(), ViewMatrix, ProjectionMatrix);
    //Renderer->PrepareShader(GetShader());
    //Renderer->DrawIndexedPrimitiveComponent(GetMesh(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
