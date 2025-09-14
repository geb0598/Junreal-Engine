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

//void UBoundingBoxComponent::DrawDebug(ID3D11DeviceContext* DC)
//{
//    // TODO: DirectX11 라인 드로우 (12개의 edge)
//    // VertexBuffer + Draw(24 vertices or 12 lines)
//}

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