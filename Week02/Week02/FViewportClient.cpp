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
    Camera = NewObject<UCameraComponent>();
}

FViewportClient::~FViewportClient()
{
}

void FViewportClient::Draw(FViewport* Viewport)
{
    if (!Viewport || !World) return;
    ACameraActor* MainCamera = World->GetCameraActor();
    FMatrix ViewMatrix{};
    FMatrix ProjectionMatrix{};
    switch (ViewportType)
    {
    case EViewportType::Perspective:
    {
        ViewMatrix = MainCamera->GetViewMatrix();
        ProjectionMatrix = MainCamera->GetProjectionMatrix();
        break;
    }
    case EViewportType::Orthographic_Top:
    case EViewportType::Orthographic_Front:
    case EViewportType::Orthographic_Left:
    case EViewportType::Orthographic_Back:
    case EViewportType::Orthographic_Bottom:
    case EViewportType::Orthographic_Right:
    {
        Camera->SetProjectionMode(ECameraProjectionMode::Orthographic);
        Camera->SetWorldLocation({0, 0, 1000}); 
        Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 90, 0 }));
        Camera->SetFOV(100);
        SetupOrthographicCamera();
        ViewMatrix = Camera->GetViewMatrix();
        ProjectionMatrix = Camera->GetProjectionMatrix();
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
            
            Camera->SetWorldLocation({ 0, 0, 1000  } );
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 90, 0 }));
            break;
        case EViewportType::Orthographic_Bottom:

            Camera->SetWorldLocation({ 0, 0, -1000 });
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, -90, 0 }));
            break;
        case EViewportType::Orthographic_Left:
            Camera->SetWorldLocation({ 0, 1000 , 0 });
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 0, -90 }));
            break;
        case EViewportType::Orthographic_Right:
            Camera->SetWorldLocation({ 0, 1000 , 0 });
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 0, 90 }));
            break;

        case EViewportType::Orthographic_Front:
            Camera->SetWorldLocation({ -1000, 0, 0});
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 0, 0 }));
            break;
        case EViewportType::Orthographic_Back:
            Camera->SetWorldLocation({ 1000, 0, 0 });
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 0, 180 }));
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
    FVector2D ViewportMousePos(static_cast<float>(X), static_cast<float>(Y));

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
        PickingCamera = World->GetCameraActor();
    }

    if (PickingCamera)
    {
        // For orthographic viewports, we need to use our local camera component's matrices instead
        AActor* PickedActor = nullptr;
        TArray<AActor*> AllActors = World->GetActors();

        if (ViewportType == EViewportType::Perspective)
        {
            // Use normal picking for perspective view
            PickedActor = CPickingSystem::PerformViewportPicking(AllActors, PickingCamera, ViewportMousePos, ViewportSize, ViewportOffset);
        }
        else
        {
            // For orthographic views, create a ray using our local camera settings
            const FMatrix View = Camera->GetViewMatrix();
            const FMatrix Proj = Camera->GetProjectionMatrix();
            const FVector CameraWorldPos = Camera->GetWorldLocation();
            const FVector CameraRight = Camera->GetRight();
            const FVector CameraUp = Camera->GetUp();
            const FVector CameraForward = Camera->GetForward();

            FRay ray = MakeRayFromViewport(View, Proj, CameraWorldPos, CameraRight, CameraUp, CameraForward,
                                           ViewportMousePos, ViewportSize, ViewportOffset);

            int pickedIndex = -1;
            float pickedT = 1e9f;

            // Perform picking with our custom ray
            for (int i = 0; i < AllActors.Num(); ++i)
            {
                AActor* Actor = AllActors[i];
                if (!Actor || Actor->GetActorHiddenInGame()) continue;

                float hitDistance;
                if (CPickingSystem::CheckActorPicking(Actor, ray, hitDistance))
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
                PickedActor = AllActors[pickedIndex];
                char buf[160];
                sprintf_s(buf, "[Ortho Pick] Hit primitive %d at t=%.3f\n", pickedIndex, pickedT);
                UE_LOG(buf);
            }
        }

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

