#include "pch.h"
#include "CameraComponent.h"

extern float CLIENTWIDTH;
extern float CLIENTHEIGHT;

UCameraComponent::UCameraComponent()
    : FieldOfView(60.0f)
    , AspectRatio(1.0f / 1.0f)
    , NearClip(0.1f)
    , FarClip(1000.0f)
    , ProjectionMode(ECameraProjectionMode::Perspective)
{
}

UCameraComponent::~UCameraComponent() {}

FMatrix UCameraComponent::GetViewMatrix() const
{
    // Robust path: View = inverse(world) under row-vector convention.
    // Use full transform matrix (translation in last row) and invert affine part.
    const FMatrix World = GetWorldTransform().ToMatrixWithScaleLocalXYZ();
    return World.InverseAffine();
}


FMatrix UCameraComponent::GetProjectionMatrix() const
{
    // 최신 종횡비 계산
    float aspect = CLIENTWIDTH / CLIENTHEIGHT;

    if (ProjectionMode == ECameraProjectionMode::Perspective)
    {
        return FMatrix::PerspectiveFovLH(FieldOfView * (PI / 180.0f),
            aspect,
            NearClip, FarClip);
    }
    else
    {
        float orthoHeight = 2.0f * tanf((FieldOfView * PI / 180.0f) * 0.5f) * 10.0f;
        float orthoWidth = orthoHeight * aspect;
        
        return FMatrix::OrthoLH(orthoWidth, orthoHeight,
            NearClip, FarClip);
    }
}
FVector UCameraComponent::GetForward() const
{
    return GetWorldTransform().Rotation.RotateVector(FVector(1, 0, 0)).GetNormalized();
}

FVector UCameraComponent::GetRight() const
{
    return GetWorldTransform().Rotation.RotateVector(FVector(0, 1, 0)).GetNormalized();
}

FVector UCameraComponent::GetUp() const
{
    return GetWorldTransform().Rotation.RotateVector(FVector(0, 0, 1)).GetNormalized();
}
