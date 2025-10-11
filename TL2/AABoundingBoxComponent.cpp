﻿//#include"pch.h"
//#include "AABoundingBoxComponent.h"
//#include "SelectionManager.h"
//#include "Line.h"   
//#include"OBoundingBoxComponent.h"
//
//UAABoundingBoxComponent::UAABoundingBoxComponent()
//    : LocalMin(FVector{}), LocalMax(FVector{}), LineColor(0.0f, 0.0f, 0.0f, 1.0f) // 노란색
//{
//}
//
//using std::max;
//using std::sqrt;
//
//void UAABoundingBoxComponent::SetFromVertices(const TArray<FVector>& Verts)
//{
//    if (Verts.empty()) return;
//
//    LocalMin = LocalMax = Verts[0];
//    for (const auto& v : Verts)
//    {
//        LocalMin = LocalMin.ComponentMin(v);
//        LocalMax = LocalMax.ComponentMax(v);
//    }
//
//    Bound = GetWorldBoundFromCube();
//}
//
//void UAABoundingBoxComponent::SetFromVertices(const TArray<FNormalVertex>& Verts)
//{
//    if (Verts.empty()) return;
//
//    LocalMin = LocalMax = Verts[0].pos;
//    for (const auto& v : Verts)
//    {
//        LocalMin = LocalMin.ComponentMin(v.pos);
//        LocalMax = LocalMax.ComponentMax(v.pos);
//    }
//    Bound = GetWorldBoundFromCube();
//}
//
//void UAABoundingBoxComponent::SetMinMax(const FBound& Bound)
//{
//    LocalMin = Bound.Min;
//    LocalMax = Bound.Max;
//}
//
//void UAABoundingBoxComponent::Render(URenderer* Renderer, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
//{
//  //  if (USelectionManager::GetInstance().GetSelectedActor() == GetOwner())
//    //{
//        TArray<FVector> Start;
//        TArray<FVector> End;
//        TArray<FVector4> Color;
//
//        Bound = GetWorldBoundFromCube();
//        CreateLineData(Bound.Min, Bound.Max, Start, End, Color);
//        Renderer->AddLines(Start, End, Color);
//   // }
//}
//
//void UAABoundingBoxComponent::TickComponent(float DeltaTime)
//{
//}
//
//FBound UAABoundingBoxComponent::GetWorldBoundFromCube()
//{
//    auto corners = GetLocalCorners();
//
//    FMatrix WorldMat=FMatrix::Identity();
//    if (GetOwner()) {
//        WorldMat = GetOwner()->GetWorldMatrix();
//        // 마지막으로 AABB를 계산했을 때의 월드 행렬과 현재 월드 행렬을 비교합니다.
//        // 행렬에 변화가 있을 경우에만 AABB를 다시 계산하여 성능을 최적화합니다. (캐싱)
//        if (WorldMat == LastWorldMat)
//        {
//            return WorldBound;
//        }
//    }
//    FVector4 MinW = corners[0] * WorldMat;
//    FVector4 MaxW = MinW;
//
//    for (auto& c : corners)
//    {
//        FVector4 wc = c * WorldMat;
//        MinW = MinW.ComponentMin(wc);
//        MaxW = MaxW.ComponentMax(wc);
//    }
//
//    LastWorldMat = WorldMat;
//    WorldBound = FBound({ MinW.X, MinW.Y, MinW.Z }, { MaxW.X, MaxW.Y, MaxW.Z});
//
//    return WorldBound;
//}
//
//FBound UAABoundingBoxComponent::GetWorldBoundFromSphere() const
//{
//    //중심 이동
//    const FVector WorldCenter = GetOwner()->GetActorLocation();
//    const FMatrix WorldMat = GetOwner()->GetWorldMatrix();
//
//    //월드 축 기준 스케일 값 계산
//    FVector WorldScaleExtents;
//    WorldScaleExtents.X = sqrtf(
//        WorldMat.M[0][0] * WorldMat.M[0][0] +
//        WorldMat.M[1][0] * WorldMat.M[1][0] + 
//        WorldMat.M[2][0] * WorldMat.M[2][0]
//    );
//    WorldScaleExtents.Y = sqrtf(
//        WorldMat.M[0][1] * WorldMat.M[0][1] +
//        WorldMat.M[1][1] * WorldMat.M[1][1] +
//        WorldMat.M[2][1] * WorldMat.M[2][1]
//    );
//    WorldScaleExtents.Z = sqrtf(
//        WorldMat.M[0][2] * WorldMat.M[0][2] +
//        WorldMat.M[1][2] * WorldMat.M[1][2] +
//        WorldMat.M[2][2] * WorldMat.M[2][2]
//    );
//
//    //최종 AABB
//    FVector Min = WorldCenter - WorldScaleExtents;
//    FVector Max = WorldCenter + WorldScaleExtents;
//    return FBound(Min, Max);
//}
//
//const FBound* UAABoundingBoxComponent::GetFBound() const
//{
//    return &Bound;
//}
//
//FOrientedBound UAABoundingBoxComponent::GetWorldOrientedBound() const
//{
//    FMatrix WorldMat = FMatrix::Identity();
//    if (GetOwner()) {
//        WorldMat = GetOwner()->GetWorldMatrix();
//    }
//
//    FVector LocalCenter = (LocalMin + LocalMax) * 0.5f;
//    FVector LocalExtents = (LocalMax - LocalMin) * 0.5f;
//
//    FVector4 WorldCenter4 = FVector4(LocalCenter.X, LocalCenter.Y, LocalCenter.Z, 1.0f) * WorldMat;
//    FVector WorldCenter = FVector(WorldCenter4.X, WorldCenter4.Y, WorldCenter4.Z);
//
//    FVector WorldExtents;
//    WorldExtents.X = std::abs(WorldMat.M[0][0] * LocalExtents.X) +
//                     std::abs(WorldMat.M[0][1] * LocalExtents.Y) +
//                     std::abs(WorldMat.M[0][2] * LocalExtents.Z);
//    WorldExtents.Y = std::abs(WorldMat.M[1][0] * LocalExtents.X) +
//                     std::abs(WorldMat.M[1][1] * LocalExtents.Y) +
//                     std::abs(WorldMat.M[1][2] * LocalExtents.Z);
//    WorldExtents.Z = std::abs(WorldMat.M[2][0] * LocalExtents.X) +
//                     std::abs(WorldMat.M[2][1] * LocalExtents.Y) +
//                     std::abs(WorldMat.M[2][2] * LocalExtents.Z);
//
//    return FOrientedBound(WorldCenter, WorldExtents, WorldMat);
//}
//
//bool UAABoundingBoxComponent::RayIntersectsOBB(const FVector& Origin, const FVector& Direction, float& Distance) const
//{
//    FOrientedBound WorldOBB = GetWorldOrientedBound();
//    return WorldOBB.RayIntersects(Origin, Direction, Distance);
//}
//
//TArray<FVector4> UAABoundingBoxComponent::GetLocalCorners() const
//{
//    return 
//    {
//        {LocalMin.X, LocalMin.Y, LocalMin.Z, 1},
//        {LocalMax.X, LocalMin.Y, LocalMin.Z, 1},
//        {LocalMin.X, LocalMax.Y, LocalMin.Z, 1},
//        {LocalMax.X, LocalMax.Y, LocalMin.Z, 1},
//        {LocalMin.X, LocalMin.Y, LocalMax.Z, 1},
//        {LocalMax.X, LocalMin.Y, LocalMax.Z, 1},
//        {LocalMin.X, LocalMax.Y, LocalMax.Z, 1},
//        {LocalMax.X, LocalMax.Y, LocalMax.Z, 1}
//    };
//}
//
//UObject* UAABoundingBoxComponent::Duplicate()
//{
//    // 부모의 Duplicate 호출 (얕은 복사)
//    UAABoundingBoxComponent* DuplicatedComponent = NewObject<UAABoundingBoxComponent>(*this);
//
//    // AABoundingBoxComponent 고유 속성 복사
//    DuplicatedComponent->Bound = this->Bound;
//    DuplicatedComponent->LocalMin = this->LocalMin;
//    DuplicatedComponent->LocalMax = this->LocalMax;
//    DuplicatedComponent->LineColor = this->LineColor;
//    DuplicatedComponent->PrimitiveType = this->PrimitiveType;
//    DuplicatedComponent->LastWorldMat = this->LastWorldMat;
//    DuplicatedComponent->WorldBound = this->WorldBound;
//
//    // 자식 컴포넌트 복제
//    DuplicatedComponent->DuplicateSubObjects();
//
//    return DuplicatedComponent;
//}
//
//void UAABoundingBoxComponent::CreateLineData(
//    const FVector& Min, const FVector& Max,
//    OUT TArray<FVector>& Start,
//    OUT TArray<FVector>& End,
//    OUT TArray<FVector4>& Color)
//{
//    // 8개 꼭짓점 정의
//    const FVector v0(Min.X, Min.Y, Min.Z);
//    const FVector v1(Max.X, Min.Y, Min.Z);
//    const FVector v2(Max.X, Max.Y, Min.Z);
//    const FVector v3(Min.X, Max.Y, Min.Z);
//    const FVector v4(Min.X, Min.Y, Max.Z);
//    const FVector v5(Max.X, Min.Y, Max.Z);
//    const FVector v6(Max.X, Max.Y, Max.Z);
//    const FVector v7(Min.X, Max.Y, Max.Z);
//
//    // 선 색상 정의
//
//
//    // --- 아래쪽 면 ---
//    Start.Add(v0); End.Add(v1); Color.Add(LineColor);
//    Start.Add(v1); End.Add(v2); Color.Add(LineColor);
//    Start.Add(v2); End.Add(v3); Color.Add(LineColor);
//    Start.Add(v3); End.Add(v0); Color.Add(LineColor);
//
//    // --- 위쪽 면 ---
//    Start.Add(v4); End.Add(v5); Color.Add(LineColor);
//    Start.Add(v5); End.Add(v6); Color.Add(LineColor);
//    Start.Add(v6); End.Add(v7); Color.Add(LineColor);
//    Start.Add(v7); End.Add(v4); Color.Add(LineColor);
//
//    // --- 옆면 기둥 ---
//    Start.Add(v0); End.Add(v4); Color.Add(LineColor);
//    Start.Add(v1); End.Add(v5); Color.Add(LineColor);
//    Start.Add(v2); End.Add(v6); Color.Add(LineColor);
//    Start.Add(v3); End.Add(v7); Color.Add(LineColor);
//}
