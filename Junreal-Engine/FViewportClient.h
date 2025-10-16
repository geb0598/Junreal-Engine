﻿#pragma once
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
    Orthographic_Bottom,    // 하단 직교 뷰
    Orthographic_Front,   // 정면 직교 뷰
    Orthographic_Left,     // 왼쪽면 직교 뷰 
    Orthographic_Right,   // 오른쪽면 직교 뷰
    Orthographic_Back     // 측면 직교 뷰
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
    virtual void Tick(float DeltaTime);

    // 입력 처리
    virtual void MouseMove(FViewport* Viewport, int32 X, int32 Y);
    virtual void MouseButtonDown(FViewport* Viewport, int32 X, int32 Y, int32 Button);
    virtual void MouseButtonUp(FViewport* Viewport, int32 X, int32 Y, int32 Button);
    virtual void MouseWheel(float DeltaSeconds);
    virtual void KeyDown(FViewport* Viewport, int32 KeyCode) {}
    virtual void KeyUp(FViewport* Viewport, int32 KeyCode) {}

    // 뷰포트 설정
    void SetViewportType(EViewportType InType) { ViewportType = InType; }
    EViewportType GetViewportType() const { return ViewportType; }

    void SetWorld(UWorld* InWorld) { World = InWorld; }
    UWorld* GetWorld() const { return World; }

    void SetCamera(ACameraActor* InCamera) { Camera = InCamera; }
    ACameraActor* GetCamera() const { return Camera; }

    // 카메라 매트릭스 계산
    FMatrix GetViewMatrix() const;


    // 뷰포트별 카메라 설정
    void SetupCameraMode();
    void SetViewModeIndex(EViewModeIndex InViewModeIndex) { ViewModeIndex = InViewModeIndex; }

    EViewModeIndex GetViewModeIndex() { return ViewModeIndex;}


protected:
    EViewportType ViewportType = EViewportType::Perspective;
    UWorld* World = nullptr;
    ACameraActor* Camera = nullptr;
    ACameraActor* ViewPortCamera = nullptr;
    int32 MouseLastX{};
    int32 MouseLastY{};
    bool bIsMouseButtonDown = false;
    bool bIsMouseRightButtonDown = false;
    static FVector CameraAddPosition;


    // 직교 뷰용 카메라 설정
    uint32 OrthographicAddXPosition;
    uint32  OrthographicAddYPosition;
    float OrthographicZoom = 30.0f;
    //뷰모드
    EViewModeIndex ViewModeIndex = EViewModeIndex::VMI_Lit;

    //원근 투영
    bool PerspectiveCameraInput = false;
    FVector PerspectiveCameraPosition;
    FQuat PerspectiveCameraRotation;
    float PerspectiveCameraFov=60;
};