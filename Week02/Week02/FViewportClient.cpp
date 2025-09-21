#include "pch.h"
#include "FViewportClient.h"
#include "FViewport.h"
#include "CameraComponent.h"
#include"CameraActor.h"
#include "World.h"
#include "Picking.h"
#include "SelectionManager.h"

FViewportClient::FViewportClient()
{
    ViewportType = EViewportType::Perspective;
    

    // 직교 뷰별 기본 카메라 설정
    SetupOrthographicCamera();
    Camera = NewObject<ACameraActor>();
}

FViewportClient::~FViewportClient()
{
}

void FViewportClient::Draw(FViewport* Viewport)
{
    if (!Viewport || !World) return;

    // 뷰포트의 실제 크기로 aspect ratio 계산
    float ViewportAspectRatio = static_cast<float>(Viewport->GetSizeX()) / static_cast<float>(Viewport->GetSizeY());
    if (Viewport->GetSizeY() == 0) ViewportAspectRatio = 1.0f; // 0으로 나누기 방지

    ACameraActor* MainCamera = World->GetCameraActor();
    FMatrix ViewMatrix{};
    FMatrix ProjectionMatrix{};
    switch (ViewportType)
    {
    case EViewportType::Perspective:
    {
        ViewMatrix = MainCamera->GetViewMatrix();
        ProjectionMatrix = MainCamera->GetProjectionMatrix(ViewportAspectRatio);
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
        Camera->GetCameraComponent()->SetFOV(100);
        SetupOrthographicCamera();
        ViewMatrix = Camera->GetViewMatrix();
        ProjectionMatrix = Camera->GetProjectionMatrix(ViewportAspectRatio);
        break;
    }
    }
    // 월드의 모든 액터들을 렌더링

    if (World)
    {
        World->RenderViewports(ViewMatrix, ProjectionMatrix);
    }
}



void FViewportClient::SetupOrthographicCamera()
{
    switch (ViewportType)
    {
        case EViewportType::Orthographic_Top:
            
            Camera->SetActorLocation({ 0, 0, 1000  } );
            Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 90, 0 }));
            break;
        case EViewportType::Orthographic_Bottom:

            Camera->SetActorLocation({ 0, 0, -1000 });
            Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, -90, 0 }));
            break;
        case EViewportType::Orthographic_Left:
            Camera->SetActorLocation({ 0, 1000 , 0 });
            Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, -90 }));
            break;
        case EViewportType::Orthographic_Right:
            Camera->SetActorLocation({ 0, 1000 , 0 });
            Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 90 }));
            break;

        case EViewportType::Orthographic_Front:
            Camera->SetActorLocation({ -1000, 0, 0 });
            Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 0 }));
            break;
        case EViewportType::Orthographic_Back:
            Camera->SetActorLocation({ 1000, 0, 0 });
            Camera->SetActorRotation(FQuat::MakeFromEuler({ 0, 0, 180 }));
            break;


    }
}

void FViewportClient::MouseButtonDown(FViewport* Viewport, int32 X, int32 Y, int32 Button)
{
    if (!Viewport || !World || Button != 0) // Only handle left mouse button
        return;

    // Get viewport size
    FVector2D ViewportSize(static_cast<float>(Viewport->GetSizeX()), static_cast<float>(Viewport->GetSizeY()));
    FVector2D ViewportOffset(static_cast<float>(Viewport->GetStartX()), static_cast<float>(Viewport->GetStartY()));

    // X, Y are already local coordinates within the viewport, convert to global coordinates for picking
    FVector2D ViewportMousePos(static_cast<float>(X) + ViewportOffset.X, static_cast<float>(Y) + ViewportOffset.Y);

    // Debug log for viewport picking
    char debugBuf[256];
    sprintf_s(debugBuf, "[Viewport %d] Local: (%.1f,%.1f) Global: (%.1f,%.1f) Size: (%.1f,%.1f) Offset: (%.1f,%.1f)\n",
        static_cast<int>(ViewportType), static_cast<float>(X), static_cast<float>(Y),
        ViewportMousePos.X, ViewportMousePos.Y, ViewportSize.X, ViewportSize.Y, ViewportOffset.X, ViewportOffset.Y);
    UE_LOG(debugBuf);

    // Get the appropriate camera for this viewport
    ACameraActor* PickingCamera = nullptr;

    if (ViewportType == EViewportType::Perspective)
    {
        // Use main camera for perspective view
        PickingCamera = World->GetCameraActor();
    }
    else
    {
        // For orthographic views, we need to use our local camera settings
        // Since we can't easily create a temporary ACameraActor, we'll create a temporary one
        // or use the existing camera but modify the picking to use our Camera component's matrices
        PickingCamera = Camera;
    }

    if (PickingCamera)
    {
        // Use the appropriate camera for this viewport type
        AActor* PickedActor = nullptr;
        TArray<AActor*> AllActors = World->GetActors();

        // 뷰포트의 실제 aspect ratio 계산
        float PickingAspectRatio = ViewportSize.X / ViewportSize.Y;
        if (ViewportSize.Y == 0) PickingAspectRatio = 1.0f; // 0으로 나누기 방지

        PickedActor = CPickingSystem::PerformViewportPicking(AllActors, PickingCamera, ViewportMousePos, ViewportSize, ViewportOffset, PickingAspectRatio);
     

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
            UUIManager::GetInstance().ResetPickedActor(); 
            // Clear selection if nothing was picked
            USelectionManager::GetInstance().ClearSelection();
        }
    }
}

