#pragma once
#include "Vector.h"
#include "InputManager.h"
#include "UEContainer.h"
#include "Enums.h"

class UStaticMeshComponent;
class AGizmoActor;
// Forward Declarations
class AActor;
class ACameraActor;

// Unreal-style simple ray type
struct alignas(16) FRay
{
    FVector Origin;
    FVector Direction; // Normalized
};

// Build A world-space ray from the current mouse position and camera/projection info.
// - InView: view matrix (row-major, row-vector convention; built by LookAtLH)
// - InProj: projection matrix created by PerspectiveFovLH in this project
FRay MakeRayFromMouse(const FMatrix& InView,
                      const FMatrix& InProj);

// Improved version: directly use camera world position and orientation
FRay MakeRayFromMouseWithCamera(const FMatrix& InView,
                                const FMatrix& InProj,
                                const FVector& CameraWorldPos,
                                const FVector& CameraRight,
                                const FVector& CameraUp,
                                const FVector& CameraForward);

// Ray-sphere intersection.
// Returns true and the closest positive T if the ray hits the sphere.
bool IntersectRaySphere(const FRay& InRay, const FVector& InCenter, float InRadius, float& OutT);

// Möller–Trumbore ray-triangle intersection.
// Returns true if hit and outputs closest positive T along the ray.
bool IntersectRayTriangleMT(const FRay& InRay,
                            const FVector& InA,
                            const FVector& InB,
                            const FVector& InC,
                            float& OutT);

/**
 * PickingSystem
 * - 액터 피킹 관련 로직을 담당하는 클래스
 */
class CPickingSystem
{
public:
    /** === 피킹 실행 === */
    static AActor* PerformPicking(const TArray<AActor*>& Actors, ACameraActor* Camera);

    // 현재 기즈모의 어떤 축에 호버링 중인지 반환 (X=1, Y=2, Z=3)
    static uint32 IsHoveringGizmo(AGizmoActor* GizmoActor, const ACameraActor* Camera);
    
    // 기즈모 드래그로 액터를 이동시키는 함수
    static void DragActorWithGizmo(AActor* Actor, AGizmoActor* GizmoActor, uint32 GizmoAxis, const FVector2D& MouseDelta, const ACameraActor* Camera, EGizmoMode InGizmoMode);
private:
    /** === 내부 헬퍼 함수들 === */
    static bool CheckActorPicking(const AActor* Actor, const FRay& Ray, float& OutDistance);
    static bool CheckGizmoComponentPicking(const UStaticMeshComponent* Component, const FRay& Ray, float& OutDistance);
};
