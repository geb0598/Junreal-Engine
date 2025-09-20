#pragma once
#include "Vector.h"
#include"Enums.h"
class FViewport;
class UWorld;
class UCameraComponent;

/**
 * @brief 뷰포트 렌더링 타입
 */
enum class EViewportType : uint8
{
    Perspective,    // 원근 뷰
    Orthographic_Top,     // 상단 직교 뷰
    Orthographic_Front,   // 정면 직교 뷰
    Orthographic_Side     // 측면 직교 뷰
};

/**
 * @brief 뷰포트 클라이언트 - UE의 FViewportClient를 모방
 */
class FViewportClient
{
public:
    FViewportClient();
    virtual ~FViewportClient();

    // 렌더링
    virtual void Draw(FViewport* Viewport);
    virtual void Tick(float DeltaTime) {}

    // 입력 처리
    virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y) {}
    virtual void MouseButtonDown(FViewport* Viewport, int32 X, int32 Y, int32 Button) {}
    virtual void MouseButtonUp(FViewport* Viewport, int32 X, int32 Y, int32 Button) {}
    virtual void KeyDown(FViewport* Viewport, int32 KeyCode) {}
    virtual void KeyUp(FViewport* Viewport, int32 KeyCode) {}

    // 뷰포트 설정
    void SetViewportType(EViewportType InType) { ViewportType = InType; }
    EViewportType GetViewportType() const { return ViewportType; }

    void SetWorld(UWorld* InWorld) { World = InWorld; }
    UWorld* GetWorld() const { return World; }

    void SetCamera(UCameraComponent* InCamera) { Camera = InCamera; }
    UCameraComponent* GetCamera() const { return Camera; }

    // 카메라 매트릭스 계산
    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix(float ViewportWidth, float ViewportHeight) const;

    // 뷰포트별 카메라 설정
    void SetupOrthographicCamera();
    void SetupPerspectiveCamera();

protected:
    EViewportType ViewportType = EViewportType::Perspective;
    UWorld* World = nullptr;
    UCameraComponent* Camera = nullptr;

    // 직교 뷰용 카메라 설정
    FVector OrthographicLocation;
    FQuat OrthographicRotation;
    float OrthographicZoom = 1000.0f;
};