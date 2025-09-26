#pragma once
#include "ShapeComponent.h"

struct FBox;
struct FBound
{
    FVector Min;
    FVector Max;

    FBound() : Min(FVector()), Max(FVector()) {}
    FBound(const FVector& InMin, const FVector& InMax) : Min(InMin), Max(InMax) {}

    // 센터 반환
    FVector GetCenter() const
    {
        return (Min + Max) * 0.5f;
    }
    // 박스의 절반 크기
    FVector GetExtent() const
    {
        return (Max - Min) * 0.5f;
    }
    bool IsInside(const FVector& Point) const
    {
        return (Point.X >= Min.X && Point.X <= Max.X &&
            Point.Y >= Min.Y && Point.Y <= Max.Y &&
            Point.Z >= Min.Z && Point.Z <= Max.Z);
    }
    bool IsIntersect(const FBox& Other) const
    {
        return (Min.X <= Other.Max.X && Max.X >= Other.Min.X &&
            Min.Y <= Other.Max.Y && Max.Y >= Other.Min.Y &&
            Min.Z <= Other.Max.Z && Max.Z >= Other.Min.Z);
    }
};

class ULine;
class UAABoundingBoxComponent :
    public UShapeComponent
{
    DECLARE_CLASS(UAABoundingBoxComponent, UShapeComponent)
public:
    UAABoundingBoxComponent();

    // 주어진 로컬 버텍스들로부터 Min/Max 계산
    void SetFromVertices(const TArray<FVector>& Verts);
    void SetFromVertices(const TArray<FNormalVertex>& Verts);
    void SetMinMax(const FBox& Box);
    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;

    // 월드 좌표계에서의 AABB 반환
    FBound GetWorldBoundFromCube() const;
    FBound GetWorldBoundFromSphere() const;

    TArray<FVector4> GetLocalCorners() const;

    void SetPrimitiveType(EPrimitiveType InType) { PrimitiveType = InType; }

private:
    void CreateLineData(
        const FVector& Min, const FVector& Max,
        OUT TArray<FVector>& Start,
        OUT TArray<FVector>& End,
        OUT TArray<FVector4>& Color);

    FVector LocalMin;
    FVector LocalMax;

    EPrimitiveType PrimitiveType = EPrimitiveType::Default;
};

