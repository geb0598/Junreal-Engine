#include "pch.h"
#include "FViewportClient.h"
#include "FViewport.h"
#include "CameraComponent.h"
#include "World.h"

FViewportClient::FViewportClient()
{
    ViewportType = EViewportType::Perspective;
    OrthographicZoom = 1000.0f;

    // 직교 뷰별 기본 카메라 설정
    SetupOrthographicCamera();
}

FViewportClient::~FViewportClient()
{
}

void FViewportClient::Draw(FViewport* Viewport)
{
    if (!Viewport || !World) return;

    // 뷰 매트릭스와 프로젝션 매트릭스 계산
    FMatrix ViewMatrix = GetViewMatrix();
    FMatrix ProjectionMatrix = GetProjectionMatrix(
        static_cast<float>(Viewport->GetSizeX()),
        static_cast<float>(Viewport->GetSizeY())
    );

    // TODO: 실제 렌더링 시스템과 연동
    // 현재는 기본 렌더링만 수행

    // 월드의 모든 액터들을 렌더링
    if (World)
    {
        // World->RenderActors(ViewMatrix, ProjectionMatrix);
    }
}

FMatrix FViewportClient::GetViewMatrix() const
{
    FVector CameraLocation;
    FQuat CameraRotation;

    if (Camera)
    {
        // 실제 카메라가 있다면 해당 카메라의 트랜스폼 사용
        CameraLocation = Camera->GetWorldLocation();
        CameraRotation = Camera->GetWorldRotation();
    }
    else
    {
        // 뷰포트 타입에 따른 기본 카메라 설정
        CameraLocation = OrthographicLocation;
        CameraRotation = OrthographicRotation;
    }

    // 카메라 방향 벡터들
    FVector Forward = CameraRotation.GetForwardVector();
    FVector Right = CameraRotation.GetRightVector();
    FVector Up = CameraRotation.GetUpVector();

    // LookAt 타겟 계산
    FVector Target = CameraLocation + Forward;

    return FMatrix::LookAtLH(CameraLocation, Target, Up);
}

FMatrix FViewportClient::GetProjectionMatrix(float ViewportWidth, float ViewportHeight) const
{
    if (ViewportWidth <= 0 || ViewportHeight <= 0)
        return FMatrix::Identity();

    switch (ViewportType)
    {
        case EViewportType::Perspective:
        {
            float FOV = DegreeToRadian(75.0f); // 75도 FOV
            float AspectRatio = ViewportWidth / ViewportHeight;
            float NearPlane = 1.0f;
            float FarPlane = 10000.0f;

            return FMatrix::PerspectiveFovLH(FOV, AspectRatio, NearPlane, FarPlane);
        }

        case EViewportType::Orthographic_Top:
        case EViewportType::Orthographic_Front:
        case EViewportType::Orthographic_Side:
        {
            float OrthoWidth = OrthographicZoom;
            float OrthoHeight = OrthographicZoom * (ViewportHeight / ViewportWidth);
            float NearPlane = -10000.0f;
            float FarPlane = 10000.0f;

            return FMatrix::OrthoLH(OrthoWidth, OrthoHeight, NearPlane, FarPlane);
        }
    }

    return FMatrix::Identity();
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
