#include "pch.h"
#include "FViewportClient.h"
#include "FViewport.h"
#include "CameraComponent.h"
#include"CameraActor.h"
#include "World.h"
#include "Picking.h"
#include "SelectionManager.h"
#include"GizmoActor.h"
FVector FViewportClient::CameraAddPosition{};
float FViewportClient::CameraWheelDelta{};
FViewportClient::FViewportClient()
{
    ViewportType = EViewportType::Perspective;


    // 직교 뷰별 기본 카메라 설정

    Camera = NewObject<ACameraActor>();
    SetupCameraMode();
}

FViewportClient::~FViewportClient()
{
}
void FViewportClient::Tick(float DeltaTime) {


}
void FViewportClient::Draw(FViewport* Viewport)
{
    if (!Viewport || !World) return;

    // 뷰포트의 실제 크기로 aspect ratio 계산
    float ViewportAspectRatio = static_cast<float>(Viewport->GetSizeX()) / static_cast<float>(Viewport->GetSizeY());
    if (Viewport->GetSizeY() == 0) ViewportAspectRatio = 1.0f; // 0으로 나누기 방지


    FMatrix ViewMatrix{};
    FMatrix ProjectionMatrix{};
    switch (ViewportType)
    {
    case EViewportType::Perspective:
    {
        ACameraActor* MainCamera = World->GetCameraActor();
        MainCamera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Perspective);

        ViewMatrix = MainCamera->GetViewMatrix();
        ProjectionMatrix = MainCamera->GetProjectionMatrix(ViewportAspectRatio);
        Camera = MainCamera;
        /*  if (World)
          {
              World->SetViewModeIndex(ViewModeIndex);
              World->RenderViewports(MainCamera, Viewport);
              World->GetGizmoActor()->Render(MainCamera, Viewport);
          }*/
        break;
    }
    case EViewportType::Orthographic_Top:
    case EViewportType::Orthographic_Front:
    case EViewportType::Orthographic_Left:
    case EViewportType::Orthographic_Back:
    case EViewportType::Orthographic_Bottom:
    case EViewportType::Orthographic_Right:
    {
        Camera->GetCameraComponent()->SetProjectionMode(ECameraProjectionMode::Orthographic);

        SetupCameraMode();
        ViewMatrix = Camera->GetViewMatrix();
        ProjectionMatrix = Camera->GetProjectionMatrix(ViewportAspectRatio);

        break;
    }
    }
    // 월드의 모든 액터들을 렌더링
    if (World)
    {
        World->SetViewModeIndex(ViewModeIndex);
        World->RenderViewports(Camera, Viewport);
        World->GetGizmoActor()->Render(Camera, Viewport);
    }

}



void FViewportClient::SetupCameraMode()
{
    switch (ViewportType)
    {
    case EViewportType::Perspective:

        Camera->SetActorLocation({ 0, 0, 0 });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 0 }));
        break;
    case EViewportType::Orthographic_Top:

        Camera->SetActorLocation({ CameraAddPosition.X, CameraAddPosition.Y, 1000 });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 90, 0 }));
        Camera->GetCameraComponent()->SetFOV(100 + CameraWheelDelta);
        break;
    case EViewportType::Orthographic_Bottom:

        Camera->SetActorLocation({ CameraAddPosition.X, CameraAddPosition.Y, -1000 });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, -90, 0 }));
        Camera->GetCameraComponent()->SetFOV(100 + CameraWheelDelta);
        break;
    case EViewportType::Orthographic_Left:
        Camera->SetActorLocation({ CameraAddPosition.X, 1000 , CameraAddPosition.Z });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, -90 }));
        Camera->GetCameraComponent()->SetFOV(100 + CameraWheelDelta);
        break;
    case EViewportType::Orthographic_Right:
        Camera->SetActorLocation({ CameraAddPosition.X, -1000, CameraAddPosition.Z });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 90 }));
        Camera->GetCameraComponent()->SetFOV(100 + CameraWheelDelta);
        break;

    case EViewportType::Orthographic_Front:
        Camera->SetActorLocation({ -1000 , CameraAddPosition.Y, CameraAddPosition.Z });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 0 }));
        Camera->GetCameraComponent()->SetFOV(100 + CameraWheelDelta);
        break;
    case EViewportType::Orthographic_Back:
        Camera->SetActorLocation({ 1000 , CameraAddPosition.Y, CameraAddPosition.Z });
        Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 180 }));
        Camera->GetCameraComponent()->SetFOV(100 + CameraWheelDelta);
        break;


    }
}
void FViewportClient::MouseMove(FViewport* Viewport, int32 X, int32 Y) {


    MouseWheel();//마우스 휠도 해줍니다 
    World->GetGizmoActor()->ProcessGizmoInteraction(Camera, Viewport, X, Y);

    if (ViewportType != EViewportType::Perspective && bIsMouseButtonDown && !World->GetGizmoActor()->GetbIsDragging()) // 직교투영이고 마우스 버튼이 눌려있을 때
    {
        int32 deltaX = X - MouseLastX;
        int32 deltaY = Y - MouseLastY;

        if (Camera && (deltaX != 0 || deltaY != 0))
        {
            float moveSpeed = 0.1f; // 픽셀 이동량을 월드 좌표로 바꿀 스케일

            // 직교 투영에서는 회전없이 그냥 화면 평면에 맞게 이동
            FVector right = Camera->GetRight(); // 수평 방향
            FVector up = Camera->GetUp();    // 수직 방향

            CameraAddPosition = CameraAddPosition - right * (deltaX * moveSpeed)
                + up * (deltaY * moveSpeed);

            // 카메라 위치 즉시 업데이트
            SetupCameraMode();
        }

        MouseLastX = X;
        MouseLastY = Y;
    }
}
void FViewportClient::MouseButtonDown(FViewport* Viewport, int32 X, int32 Y, int32 Button)
{
    if (!Viewport || !World || Button != 0) // Only handle left mouse button
        return;

    // 마우스 위치 초기화 및 드래그 시작
    MouseLastX = X;
    MouseLastY = Y;
    bIsMouseButtonDown = true;

    // Get viewport size
    FVector2D ViewportSize(static_cast<float>(Viewport->GetSizeX()), static_cast<float>(Viewport->GetSizeY()));
    FVector2D ViewportOffset(static_cast<float>(Viewport->GetStartX()), static_cast<float>(Viewport->GetStartY()));

    // X, Y are already local coordinates within the viewport, convert to global coordinates for picking
    FVector2D ViewportMousePos(static_cast<float>(X) + ViewportOffset.X, static_cast<float>(Y) + ViewportOffset.Y);

    // Debug log for viewport picking
    //char debugBuf[256];
    //sprintf_s(debugBuf, "[Viewport %d] Local: (%.1f,%.1f) Global: (%.1f,%.1f) Size: (%.1f,%.1f) Offset: (%.1f,%.1f)\n",
    //    static_cast<int>(ViewportType), static_cast<float>(X), static_cast<float>(Y),
    //    ViewportMousePos.X, ViewportMousePos.Y, ViewportSize.X, ViewportSize.Y, ViewportOffset.X, ViewportOffset.Y);
    //UE_LOG(debugBuf);

    // Get the appropriate camera for this viewport


    if (Camera)
    {
        // Use the appropriate camera for this viewport type
        AActor* PickedActor = nullptr;
        TArray<AActor*> AllActors = World->GetActors();

        // 뷰포트의 실제 aspect ratio 계산
        float PickingAspectRatio = ViewportSize.X / ViewportSize.Y;
        if (ViewportSize.Y == 0) PickingAspectRatio = 1.0f; // 0으로 나누기 방지

        PickedActor = CPickingSystem::PerformViewportPicking(AllActors, Camera, ViewportMousePos, ViewportSize, ViewportOffset, PickingAspectRatio);


        if (PickedActor)
        {
            USelectionManager::GetInstance().SelectActor(PickedActor);
            UUIManager::GetInstance().SetPickedActor(PickedActor);
            if (World->GetGizmoActor())
            {
                World->GetGizmoActor()->SetTargetActor(PickedActor);
                World->GetGizmoActor()->SetActorLocation(PickedActor->GetActorLocation());
            }
        }
        else
        {
            if (World->GetGizmoActor()->GetbIsHovering()) {
                return;
            }
            UUIManager::GetInstance().ResetPickedActor();
            // Clear selection if nothing was picked
            USelectionManager::GetInstance().ClearSelection();
        }
    }
}

void FViewportClient::MouseButtonUp(FViewport* Viewport, int32 X, int32 Y, int32 Button)
{
    if (Button == 0) // Left mouse button
    {
        bIsMouseButtonDown = false;
    }
}

void FViewportClient::MouseWheel()
{
    if (!Camera) return;

    UCameraComponent* CameraComponent = Camera->GetCameraComponent();
    if (!CameraComponent) return;
    float WheelDelta = UInputManager::GetInstance().GetMouseWheelDelta();
    // FOV 변경 속도 조절
    float FOVChangeSpeed = 3.0f;
    float currentFOV = CameraComponent->GetFOV();
    CameraWheelDelta += -(WheelDelta * FOVChangeSpeed);


}

