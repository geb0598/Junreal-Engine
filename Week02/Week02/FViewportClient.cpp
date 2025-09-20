#include "pch.h"
#include "FViewportClient.h"
#include "FViewport.h"
#include "CameraComponent.h"
#include"CameraActor.h"
#include "World.h"

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

