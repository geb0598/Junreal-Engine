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
    case EViewportType::Orthographic_Side:
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
    // TODO: 실제 렌더링 시스템과 연동
    // 현재는 기본 렌더링만 수행

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
            Camera->SetWorldLocation({ 0, 0, 1000 });
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 90, 0 }));
            break;

        case EViewportType::Orthographic_Front:
            Camera->SetWorldLocation({ -1000, 0, 0});
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 0, 0 }));
            break;

        case EViewportType::Orthographic_Side:
            Camera->SetWorldLocation({ 0, 1000, 0 });
            Camera->SetWorldRotation(FQuat::MakeFromEuler({ 0, 0, -90 }));
            break;
    }
}

void FViewportClient::SetupPerspectiveCamera()
{
    if (ViewportType == EViewportType::Perspective)
    {
        // 원근 뷰 기본 설정
        OrthographicLocation = FVector(-500, -500, 500);
        OrthographicRotation = FQuat::MakeFromEuler(FVector(-30, 45, 0));
    }
}
