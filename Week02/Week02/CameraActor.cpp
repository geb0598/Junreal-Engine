#include "CameraActor.h"
#include "SceneComponent.h"
#include "CameraComponent.h"
#include "ObjectFactory.h"


ACameraActor::ACameraActor()
{
    Name = "Camera Actor";
    // 카메라 컴포넌트
    CameraComponent = NewObject<UCameraComponent>();
    CameraComponent->SetupAttachment(RootComponent);
    Components.Add(CameraComponent);
}

ACameraActor::~ACameraActor()
{
    if (CameraComponent)
    {
        ObjectFactory::DeleteObject(CameraComponent);
    }
    CameraComponent = nullptr;
}

FMatrix ACameraActor::GetViewMatrix() const
{
    return CameraComponent ? CameraComponent->GetViewMatrix() : FMatrix::Identity();
}

FMatrix ACameraActor::GetProjectionMatrix() const
{
    return CameraComponent ? CameraComponent->GetProjectionMatrix() : FMatrix::Identity();
}

FMatrix ACameraActor::GetViewProjectionMatrix() const
{
    return GetViewMatrix() * GetProjectionMatrix();
}

FVector ACameraActor::GetForward() const
{
    return CameraComponent ? CameraComponent->GetForward() : FVector(1, 0, 0);
}

FVector ACameraActor::GetRight() const
{
    return CameraComponent ? CameraComponent->GetRight() : FVector(0, 1, 0);
}

FVector ACameraActor::GetUp() const
{
    return CameraComponent ? CameraComponent->GetUp() : FVector(0, 0, 1);
}
