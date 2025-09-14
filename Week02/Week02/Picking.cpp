#include "pch.h"

#include "Picking.h"
#include "Actor.h"
#include "StaticMeshActor.h"
#include "StaticMeshComponent.h"
#include "StaticMesh.h"
#include "CameraActor.h"
#include "MeshLoader.h"
#include"Vector.h"
#include "SelectionManager.h"
#include <cmath>

#include "GizmoActor.h"
#include "GizmoScaleComponent.h"
#include "GizmoRotateComponent.h"
#include "GizmoArrowComponent.h"
#include "UI/GlobalConsole.h"

FRay MakeRayFromMouse(const FMatrix& InView,
                      const FMatrix& InProj)
{
    // 1) Mouse to NDC (DirectX viewport convention: origin top-left)
    //    Query current screen size from InputManager
    FVector2D screen = UInputManager::GetInstance().GetScreenSize();
    float viewportW = (screen.X > 1.0f) ? screen.X : 1.0f;
    float viewportH = (screen.Y > 1.0f) ? screen.Y : 1.0f;

    const FVector2D MousePosition = UInputManager::GetInstance().GetMousePosition();
    const float NdcX = (2.0f * MousePosition.X / viewportW) - 1.0f;
    const float NdcY = 1.0f - (2.0f * MousePosition.Y / viewportH);

    // 2) View-space direction using projection scalars (PerspectiveFovLH)
    // InProj.M[0][0] = XScale, InProj.M[1][1] = YScale
    const float XScale = InProj.M[0][0];
    const float YScale = InProj.M[1][1];
    const float ViewDirX = NdcX / (XScale == 0.0f ? 1.0f : XScale);
    const float ViewDirY = NdcY / (YScale == 0.0f ? 1.0f : YScale);
    const float ViewDirZ = 1.0f; // Forward in view space

    // 3) Extract camera basis/position from InView (row-vector convention: basis in rows)
    const FVector Right = FVector(InView.M[0][0], InView.M[0][1], InView.M[0][2]);
    const FVector Up = FVector(InView.M[1][0], InView.M[1][1], InView.M[1][2]);
    const FVector Forward = FVector(InView.M[2][0], InView.M[2][1], InView.M[2][2]);
    const FVector t = FVector(InView.M[3][0], InView.M[3][1], InView.M[3][2]);
    // = (-dot(Right,Eye), -dot(Up,Eye), -dot(Fwd,Eye))
    const FVector Eye = (Right * (-t.X)) + (Up * (-t.Y)) + (Forward * (-t.Z));

    // 4) To world space
    const FVector WorldDirection = (Right * ViewDirX + Up * ViewDirY + Forward * ViewDirZ).GetSafeNormal();

    FRay Ray;
    Ray.Origin = Eye;
    Ray.Direction = WorldDirection;
    return Ray;
}

FRay MakeRayFromMouseWithCamera(const FMatrix& InView,
                                const FMatrix& InProj,
                                const FVector& CameraWorldPos,
                                const FVector& CameraRight,
                                const FVector& CameraUp,
                                const FVector& CameraForward)
{
    // 1) Mouse to NDC (DirectX viewport convention: origin top-left)
    FVector2D screen = UInputManager::GetInstance().GetScreenSize();
    float viewportW = (screen.X > 1.0f) ? screen.X : 1.0f;
    float viewportH = (screen.Y > 1.0f) ? screen.Y : 1.0f;

    const FVector2D MousePosition = UInputManager::GetInstance().GetMousePosition();
    const float NdcX = (2.0f * MousePosition.X / viewportW) - 1.0f;
    const float NdcY = 1.0f - (2.0f * MousePosition.Y / viewportH);

    // 2) View-space direction using projection scalars
    const float XScale = InProj.M[0][0];
    const float YScale = InProj.M[1][1];
    const float ViewDirX = NdcX / (XScale == 0.0f ? 1.0f : XScale);
    const float ViewDirY = NdcY / (YScale == 0.0f ? 1.0f : YScale);
    const float ViewDirZ = 1.0f; // Forward in view space

    // 3) Use camera's actual world-space orientation vectors
    // Transform view direction to world space using camera's real orientation
    const FVector WorldDirection = (CameraRight * ViewDirX + CameraUp * ViewDirY + CameraForward * ViewDirZ).
        GetSafeNormal();

    FRay Ray;
    Ray.Origin = CameraWorldPos;
    Ray.Direction = WorldDirection;
    return Ray;
}

bool IntersectRaySphere(const FRay& InRay, const FVector& InCenter, float InRadius, float& OutT)
{
    // Solve ||(RayOrigin + T*RayDir) - Center||^2 = Radius^2
    const FVector OriginToCenter = InRay.Origin - InCenter;
    const float QuadraticA = FVector::Dot(InRay.Direction, InRay.Direction); // Typically 1 for normalized ray
    const float QuadraticB = 2.0f * FVector::Dot(OriginToCenter, InRay.Direction);
    const float QuadraticC = FVector::Dot(OriginToCenter, OriginToCenter) - InRadius * InRadius;

    const float Discriminant = QuadraticB * QuadraticB - 4.0f * QuadraticA * QuadraticC;
    if (Discriminant < 0.0f)
    {
        return false;
    }

    const float SqrtD = std::sqrt(Discriminant >= 0.0f ? Discriminant : 0.0f);
    const float Inv2A = 1.0f / (2.0f * QuadraticA);
    const float T0 = (-QuadraticB - SqrtD) * Inv2A;
    const float T1 = (-QuadraticB + SqrtD) * Inv2A;

    // Pick smallest positive T
    const float ClosestT = (T0 > 0.0f) ? T0 : T1;
    if (ClosestT <= 0.0f)
    {
        return false;
    }

    OutT = ClosestT;
    return true;
}

bool IntersectRayTriangleMT(const FRay& InRay,
                            const FVector& InA,
                            const FVector& InB,
                            const FVector& InC,
                            float& OutT)
{
    const float Epsilon = KINDA_SMALL_NUMBER;

    const FVector Edge1 = InB - InA;
    const FVector Edge2 = InC - InA;

    const FVector Perpendicular = FVector::Cross(InRay.Direction, Edge2);
    const float Determinant = FVector::Dot(Edge1, Perpendicular);

    if (Determinant > -Epsilon && Determinant < Epsilon)
        return false; // Parallel to triangle

    const float InvDeterminant = 1.0f / Determinant;
    const FVector OriginToA = InRay.Origin - InA;
    const float U = InvDeterminant * FVector::Dot(OriginToA, Perpendicular);
    if (U < -Epsilon || U > 1.0f + Epsilon)
        return false;

    const FVector CrossQ = FVector::Cross(OriginToA, Edge1);
    const float V = InvDeterminant * FVector::Dot(InRay.Direction, CrossQ);
    if (V < -Epsilon || (U + V) > 1.0f + Epsilon)
        return false;

    // Compute T to find out where the intersection point is on the ray
    const float Distance = InvDeterminant * FVector::Dot(Edge2, CrossQ);

    if (Distance > Epsilon) // ray intersection
    {
        OutT = Distance;
        return true;
    }
    return false;
}

// PickingSystem 구현
AActor* CPickingSystem::PerformPicking(const TArray<AActor*>& Actors, ACameraActor* Camera)
{
    if (!Camera) return nullptr;

    // 레이 생성 - 카메라 위치와 방향을 직접 전달
    const FMatrix View = Camera->GetViewMatrix();
    const FMatrix Proj = Camera->GetProjectionMatrix();
    const FVector CameraWorldPos = Camera->GetActorLocation();
    const FVector CameraRight = Camera->GetRight();
    const FVector CameraUp = Camera->GetUp();
    const FVector CameraForward = Camera->GetForward();
    FRay ray = MakeRayFromMouseWithCamera(View, Proj, CameraWorldPos, CameraRight, CameraUp, CameraForward);

    int pickedIndex = -1;
    float pickedT = 1e9f;

    // 모든 액터에 대해 피킹 테스트
    for (int i = 0; i < Actors.Num(); ++i)
    {
        AActor* Actor = Actors[i];
        if (!Actor) continue;

        float hitDistance;
        if (CheckActorPicking(Actor, ray, hitDistance))
        {
            if (hitDistance < pickedT)
            {
                pickedT = hitDistance;
                pickedIndex = i;
            }
        }
    }

    if (pickedIndex >= 0)
    {
        char buf[160];
        sprintf_s(buf, "[Pick] Hit primitive %d at t=%.3f (Speed=NORMAL)\n", pickedIndex, pickedT);
        UE_LOG(buf);
        return Actors[pickedIndex];
    }
    else
    {
        UE_LOG("[Pick] No hit (Speed=FAST)\n");
        return nullptr;
    }
}

uint32 CPickingSystem::IsHoveringGizmo(AGizmoActor* GizmoTransActor, const ACameraActor* Camera)
{
    if (!GizmoTransActor || !Camera)
        return 0;
    
    // 카메라에서 마우스 포지션으로의 레이 생성
    const FMatrix View = Camera->GetViewMatrix();
    const FMatrix Proj = Camera->GetProjectionMatrix();
    const FVector CameraWorldPos = Camera->GetActorLocation();
    const FVector CameraRight = Camera->GetRight();
    const FVector CameraUp = Camera->GetUp();
    const FVector CameraForward = Camera->GetForward();
    FRay Ray = MakeRayFromMouseWithCamera(View, Proj, CameraWorldPos, CameraRight, CameraUp, CameraForward);
    
    uint32 ClosestAxis = 0;
    float ClosestDistance = 1e9f;
    float HitDistance;
    
    // X축 화살표 검사
    //Cast<UStaticMeshComponent>(GizmoTransActor->GetArrowX());
    
    switch (GizmoTransActor->GetMode())
    {
        case EGizmoMode::Translate:
            if (UStaticMeshComponent* ArrowX = Cast<UStaticMeshComponent>(GizmoTransActor->GetArrowX()))
            {
                if (CheckGizmoComponentPicking(ArrowX, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 1;
                    }
                }
            }

            // Y축 화살표 검사  
            if (UStaticMeshComponent* ArrowY = Cast<UStaticMeshComponent>(GizmoTransActor->GetArrowY()))
            {
                if (CheckGizmoComponentPicking(ArrowY, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 2;
                    }
                }
            }

            // Z축 화살표 검사
            if (UStaticMeshComponent* ArrowZ = Cast<UStaticMeshComponent>(GizmoTransActor->GetArrowZ()))
            {
                if (CheckGizmoComponentPicking(ArrowZ, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 3;
                    }
                }
            }
            break;
        case EGizmoMode::Scale: 
            if (UStaticMeshComponent* ScaleX = Cast<UStaticMeshComponent>(GizmoTransActor->GetScaleX()))
            {
                if (CheckGizmoComponentPicking(ScaleX, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 1;
                    }
                }
            }

            // Y축 화살표 검사  
            if (UStaticMeshComponent* ScaleY = Cast<UStaticMeshComponent>(GizmoTransActor->GetScaleY()))
            {
                if (CheckGizmoComponentPicking(ScaleY, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 2;
                    }
                }
            }

            // Z축 화살표 검사
            if (UStaticMeshComponent* ScaleZ = Cast<UStaticMeshComponent>(GizmoTransActor->GetScaleZ()))
            {
                if (CheckGizmoComponentPicking(ScaleZ, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 3;
                    }
                }
            }
            break;
        case EGizmoMode::Rotate:
            if (UStaticMeshComponent* RotateX = Cast<UStaticMeshComponent>(GizmoTransActor->GetRotateX()))
            {
                if (CheckGizmoComponentPicking(RotateX, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 1;
                    }
                }
            }

            // Y축 화살표 검사  
            if (UStaticMeshComponent* RotateY = Cast<UStaticMeshComponent>(GizmoTransActor->GetRotateY()))
            {
                if (CheckGizmoComponentPicking(RotateY, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 2;
                    }
                }
            }

            // Z축 화살표 검사
            if (UStaticMeshComponent* RotateZ = Cast<UStaticMeshComponent>(GizmoTransActor->GetRotateZ()))
            {
                if (CheckGizmoComponentPicking(RotateZ, Ray, HitDistance))
                {
                    if (HitDistance < ClosestDistance)
                    {
                        ClosestDistance = HitDistance;
                        ClosestAxis = 3;
                    }
                }
            }
            break;
    default:
        break;
    }
   
   
  
    return ClosestAxis;
}

void CPickingSystem::DragActorWithGizmo(AActor* Actor, AGizmoActor*  GizmoActor,uint32 GizmoAxis, const FVector2D& MouseDelta, const ACameraActor* Camera, EGizmoMode InGizmoMode)
{
    
    if (!Actor || !Camera || GizmoAxis == 0) 
        return;
    GizmoActor->OnDrag(Actor, GizmoAxis, MouseDelta.X, MouseDelta.Y, Camera);
    // 마우스 델타를 월드 공간 이동량으로 변환
    //FVector WorldMovement = FVector();
    //
    //// 화면 크기 정보 가져오기
    //FVector2D ScreenSize = CInputManager::GetInstance().GetScreenSize();
    //float Sensitivity = 0.02f; // 이동 민감도 조절
    //
    //// 카메라의 월드 공간 방향 벡터들
    //const FVector CameraRight = Camera->GetRight();
    //const FVector CameraUp = Camera->GetUp();
    //const FVector CameraForward = Camera->GetForward();
    //
    //float Movement = 0.0f;
    //float RightDot = 0.0f;
    //float UpDot = 0.0f;

    //switch (GizmoAxis)
    //{
    //case 1: // X축
    //    {
    //        // X축 방향으로 이동 (월드 공간 X축 = (1,0,0))
    //        FVector WorldXAxis = FVector(1.0f, 0.0f, 0.0f);
    //        
    //        // 카메라 Right 벡터와 월드 X축의 내적으로 마우스 X 이동을 월드 X축 이동으로 변환
    //        RightDot = FVector::Dot(CameraRight, WorldXAxis);
    //        UpDot = FVector::Dot(CameraUp, WorldXAxis);
    //        
    //        Movement = (MouseDelta.X * RightDot + MouseDelta.Y * UpDot) * Sensitivity;
    //        WorldMovement = WorldXAxis * Movement;
    //    }
    //    break;
    //    
    //case 2: // Y축
    //    {
    //        // Y축 방향으로 이동 (월드 공간 Y축 = (0,1,0))
    //        FVector WorldYAxis = FVector(0.0f, 1.0f, 0.0f);
    //        
    //        RightDot = FVector::Dot(CameraRight, WorldYAxis);
    //        UpDot = FVector::Dot(CameraUp, WorldYAxis);
    //        
    //        Movement = (MouseDelta.X * RightDot + MouseDelta.Y * UpDot) * Sensitivity;
    //        WorldMovement = -WorldYAxis * Movement;
    //    }
    //    break;
    //    
    //case 3: // Z축
    //    {

    //        FVector WorldZAxis = FVector(0.0f, 0.0f, 1.0f);
    //        
    //        RightDot = FVector::Dot(CameraRight, WorldZAxis);
    //        UpDot = FVector::Dot(CameraUp, WorldZAxis);
    //        
    //        Movement = (MouseDelta.X * RightDot + MouseDelta.Y * UpDot) * Sensitivity;
    //        WorldMovement = WorldZAxis * Movement;
    //    }
    //    break;
    //}
    //
    //
    //switch (InGizmoMode)
    //{
    //case EGizmoMode::Translate:
    //{
    //    FVector CurrentLocation = Actor->GetActorLocation();
    //    FVector NewLocation = CurrentLocation + WorldMovement;
    //    UE_LOG("MoveMent : %f", Movement);
    //    Actor->SetActorLocation(NewLocation);
    //    break;
    //}
    //case EGizmoMode::Scale:
    //{
    //    FVector CurrentScale = Actor->GetActorScale();
    //    FVector NewScale = CurrentScale + WorldMovement;
    //    Actor->SetActorScale(NewScale);
    //    break;
    //}
    //case EGizmoMode::Rotate:
    //{
    //    float RotationSpeed = 0.005f; // 민감도 조정 (라디안 단위)
    //    float DeltaAngleX = MouseDelta.X * RotationSpeed;
    //    float DeltaAngleY = MouseDelta.Y * RotationSpeed;

    //    // 카메라 기준 벡터
    //    FVector CameraRight = Camera->GetRight();
    //    FVector CameraUp = Camera->GetUp();

    //    FQuat DeltaQuat = FQuat::Identity();

    //    switch (GizmoAxis)
    //    {
    //    case 1: // X축 회전
    //    {
    //        // 마우스 X → 카메라 Up 축 기반
    //        FQuat RotByX = MakeQuatFromAxisAngle(FVector(-1, 0, 0), DeltaAngleX);
    //        // 마우스 Y → 카메라 Right 축 기반
    //        FQuat RotByY = MakeQuatFromAxisAngle(FVector(-1, 0, 0), DeltaAngleY);
    //        DeltaQuat = RotByX * RotByY;
    //        break;
    //    }
    //    case 2: // Y축 회전
    //    {
    //        FQuat RotByX = MakeQuatFromAxisAngle(FVector(0, -1, 0), DeltaAngleX);
    //        FQuat RotByY = MakeQuatFromAxisAngle(FVector(0, -1, 0), DeltaAngleY);
    //        DeltaQuat = RotByX * RotByY;
    //        break;
    //    }
    //    case 3: // Z축 회전
    //    {
    //        FQuat RotByX = MakeQuatFromAxisAngle(FVector(0, 0, -1), DeltaAngleX);
    //        FQuat RotByY = MakeQuatFromAxisAngle(FVector(0, 0, -1), DeltaAngleY);
    //        DeltaQuat = RotByX * RotByY;
    //        break;
    //    }
    //    }

    //    FQuat CurrentRot = Actor->GetActorRotation();
    //    FQuat NewRot = DeltaQuat * CurrentRot; // 월드 기준 회전
    //    Actor->SetActorRotation(NewRot);
    //    break;
    //}

    //default:
    //    break;
    //}
}


bool CPickingSystem::CheckGizmoComponentPicking(const UStaticMeshComponent* Component, const FRay& Ray, float& OutDistance)
{
    if (!Component) return false;

    FMeshData* MeshData = *(UMeshLoader::GetInstance().GetMeshCache())->Find(
        Component->GetStaticMesh()->GetFilePath()
    );

    if (!MeshData) return false;

    // 피킹 계산에는 Component의 WorldMatrix 사용
    FMatrix M = Component->GetWorldMatrix();

    auto TransformPoint = [&](float X, float Y, float Z) -> FVector
        {
            // row-vector (v^T) * M 방식으로 월드 변환해야 Translation이 반영됨
            // out = [X Y Z 1] * M
            FVector4 V4(X, Y, Z, 1.0f);
            FVector4 Out;
            Out.X = V4.X * M.M[0][0] + V4.Y * M.M[1][0] + V4.Z * M.M[2][0] + V4.W * M.M[3][0];
            Out.Y = V4.X * M.M[0][1] + V4.Y * M.M[1][1] + V4.Z * M.M[2][1] + V4.W * M.M[3][1];
            Out.Z = V4.X * M.M[0][2] + V4.Y * M.M[1][2] + V4.Z * M.M[2][2] + V4.W * M.M[3][2];
            Out.W = V4.X * M.M[0][3] + V4.Y * M.M[1][3] + V4.Z * M.M[2][3] + V4.W * M.M[3][3];
            return FVector(Out.X, Out.Y, Out.Z);
        };

    float ClosestT = 1e9f;
    bool bHasHit = false;

    // 인덱스가 있는 경우
    if (MeshData->Indices.Num() >= 3)
    {
        uint32 IndexNum = MeshData->Indices.Num();
        for (uint32 Idx = 0; Idx + 2 < IndexNum; Idx += 3)
        {
            const FVertexSimple& V0 = MeshData->Vertices[MeshData->Indices[Idx + 0]];
            const FVertexSimple& V1 = MeshData->Vertices[MeshData->Indices[Idx + 1]];
            const FVertexSimple& V2 = MeshData->Vertices[MeshData->Indices[Idx + 2]];

            FVector A = TransformPoint(V0.X, V0.Y, V0.Z);
            FVector B = TransformPoint(V1.X, V1.Y, V1.Z);
            FVector C = TransformPoint(V2.X, V2.Y, V2.Z);

            float THit;
            if (IntersectRayTriangleMT(Ray, A, B, C, THit))
            {
                if (THit < ClosestT)
                {
                    ClosestT = THit;
                    bHasHit = true;
                }
            }
        }
    }
    // 인덱스가 없는 경우 (순차적 삼각형)
    else if (MeshData->Vertices.Num() >= 3)
    {
        uint32 VertexNum = MeshData->Vertices.Num();
        for (uint32 Idx = 0; Idx + 2 < VertexNum; Idx += 3)
        {
            const FVertexSimple& V0 = MeshData->Vertices[Idx + 0];
            const FVertexSimple& V1 = MeshData->Vertices[Idx + 1];
            const FVertexSimple& V2 = MeshData->Vertices[Idx + 2];

            FVector A = TransformPoint(V0.X, V0.Y, V0.Z);
            FVector B = TransformPoint(V1.X, V1.Y, V1.Z);
            FVector C = TransformPoint(V2.X, V2.Y, V2.Z);

            float THit;
            if (IntersectRayTriangleMT(Ray, A, B, C, THit))
            {
                if (THit < ClosestT)
                {
                    ClosestT = THit;
                    bHasHit = true;
                }
            }
        }
    }

    if (bHasHit)
    {
        OutDistance = ClosestT;
        return true;
    }

    return false;
}

bool CPickingSystem::CheckActorPicking(const AActor* Actor, const FRay& Ray, float& OutDistance)
{
    if (!Actor) return false;

    // 메시 데이터 조회
    // TODO: MeshLoader에서 Vertex 가져올 때 중복 방지 처리 해야 함

    for (auto SceneComponent : Actor->GetComponents())
    {
        if (UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(SceneComponent))
        {
            FMeshData* MeshData = *(UMeshLoader::GetInstance().GetMeshCache())->Find(
                StaticMeshComponent->GetStaticMesh()->GetFilePath()
            );

            if (!MeshData) return false;

            // 피킹 계산에는 Actor의 WorldMatrix 사용
            FMatrix M = StaticMeshComponent->GetWorldMatrix();

            auto TransformPoint = [&](float x, float y, float z) -> FVector
                {
                    // row-vector (v^T) * M 방식으로 월드 변환해야 Translation이 반영됨
                    // out = [X Y Z 1] * M
                    FVector4 v4(x, y, z, 1.0f);
                    FVector4 out;
                    out.X = v4.X * M.M[0][0] + v4.Y * M.M[1][0] + v4.Z * M.M[2][0] + v4.W * M.M[3][0];
                    out.Y = v4.X * M.M[0][1] + v4.Y * M.M[1][1] + v4.Z * M.M[2][1] + v4.W * M.M[3][1];
                    out.Z = v4.X * M.M[0][2] + v4.Y * M.M[1][2] + v4.Z * M.M[2][2] + v4.W * M.M[3][2];
                    out.W = v4.X * M.M[0][3] + v4.Y * M.M[1][3] + v4.Z * M.M[2][3] + v4.W * M.M[3][3];
                    return FVector(out.X, out.Y, out.Z);
                };

            float closestT = 1e9f;
            bool hasHit = false;

            // 인덱스가 있는 경우
            if (MeshData->Indices.Num() >= 3)
            {
                uint32 IndexNum = MeshData->Indices.Num();
                for (uint32 idx = 0; idx + 2 < IndexNum; idx += 3)
                {
                    const FVertexSimple& v0 = MeshData->Vertices[MeshData->Indices[idx + 0]];
                    const FVertexSimple& v1 = MeshData->Vertices[MeshData->Indices[idx + 1]];
                    const FVertexSimple& v2 = MeshData->Vertices[MeshData->Indices[idx + 2]];

                    FVector a = TransformPoint(v0.X, v0.Y, v0.Z);
                    FVector b = TransformPoint(v1.X, v1.Y, v1.Z);
                    FVector c = TransformPoint(v2.X, v2.Y, v2.Z);

                    float tHit;
                    if (IntersectRayTriangleMT(Ray, a, b, c, tHit))
                    {
                        if (tHit < closestT)
                        {
                            closestT = tHit;
                            hasHit = true;
                        }
                    }
                }
            }
            // 인덱스가 없는 경우 (순차적 삼각형)
            else if (MeshData->Vertices.Num() >= 3)
            {
                uint32 VertexNum = MeshData->Vertices.Num();
                for (uint32 idx = 0; idx + 2 < VertexNum; idx += 3)
                {
                    const FVertexSimple& v0 = MeshData->Vertices[idx + 0];
                    const FVertexSimple& v1 = MeshData->Vertices[idx + 1];
                    const FVertexSimple& v2 = MeshData->Vertices[idx + 2];

                    FVector a = TransformPoint(v0.X, v0.Y, v0.Z);
                    FVector b = TransformPoint(v1.X, v1.Y, v1.Z);
                    FVector c = TransformPoint(v2.X, v2.Y, v2.Z);

                    float tHit;
                    if (IntersectRayTriangleMT(Ray, a, b, c, tHit))
                    {
                        if (tHit < closestT)
                        {
                            closestT = tHit;
                            hasHit = true;
                        }
                    }
                }
            }

            if (hasHit)
            {
                OutDistance = closestT;
                return true;
            }
        }
    }


    return false;
}
