﻿#pragma once
#include "SceneComponent.h"

enum class ECameraProjectionMode
{
    Perspective,
    Orthographic
};

class UCameraComponent : public USceneComponent
{
public:
    DECLARE_CLASS(UCameraComponent, USceneComponent)
    UCameraComponent();

protected:
    ~UCameraComponent() override;

public:
    // Projection settings
    void SetFOV(float NewFOV) { FieldOfView = NewFOV; }
    void SetAspectRatio(float NewAspect) { AspectRatio = NewAspect; }
    void SetClipPlanes(float NewNear, float NewFar) { NearClip = NewNear; FarClip = NewFar; }
    void SetNearClipPlane(float NewNear) { NearClip = NewNear; }
    void SetFarClipPlane(float NewFar) { FarClip = NewFar; }
    void SetProjectionMode(ECameraProjectionMode Mode) { ProjectionMode = Mode; }
    void SetZoomFactor(float InZoomFactor) { ZooMFactor = InZoomFactor; };
    
    float GetFOV() const { return FieldOfView; }
    float GetAspectRatio() const { return AspectRatio; }
    float GetNearClip() const { return NearClip; }
    float GetFarClip() const { return FarClip; }
    float GetZoomFactor()const { return ZooMFactor; };
    ECameraProjectionMode GetProjectionMode() const { return ProjectionMode; }

    // Matrices
    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix() const;
    FMatrix GetProjectionMatrix(float ViewportAspectRatio) const;
    FMatrix GetProjectionMatrix(float ViewportAspectRatio, FViewport* Viewport) const;//직교 투영 화면 heig조정에 대응하기 위한 코드 
    // Directions in world space
    FVector GetForward() const;
    FVector GetRight() const;
    FVector GetUp() const;

private:
    float FieldOfView;   // degrees
    float AspectRatio;
    float NearClip;
    float FarClip;

    float ZooMFactor;


    ECameraProjectionMode ProjectionMode;
};

