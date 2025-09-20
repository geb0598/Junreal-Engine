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
            // 상단 뷰 (Y축을 위로)
            OrthographicLocation = FVector(0, 0, 1000);
            OrthographicRotation = FQuat::MakeFromEuler(FVector(-90, 0, 0)); // X축으로 -90도 회전
            break;

        case EViewportType::Orthographic_Front:
            // 정면 뷰 (Z축을 위로)
            OrthographicLocation = FVector(-1000, 0, 0);
            OrthographicRotation = FQuat::MakeFromEuler(FVector(0, -90, 0)); // Y축으로 -90도 회전
            break;

        case EViewportType::Orthographic_Side:
            // 측면 뷰 (Z축을 위로)
            OrthographicLocation = FVector(0, -1000, 0);
            OrthographicRotation = FQuat::MakeFromEuler(FVector(0, 0, 0)); // 기본 방향
            break;

        case EViewportType::Perspective:
            // 원근 뷰는 기본 카메라 위치
            OrthographicLocation = FVector(-500, -500, 500);
            OrthographicRotation = FQuat::MakeFromEuler(FVector(-30, 45, 0));
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
