#pragma once
#include "SceneComponent.h"

class UExponentialHeightFogComponent : public USceneComponent
{

public:
    DECLARE_CLASS(UExponentialHeightFogComponent, USceneComponent);

    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Projection, FViewport* Viewport);
    UObject* Duplicate() override;
    void DuplicateSubObjects() override;
private:
    float FogDensity = 1.0f;
    float FogHeightFalloff = 1.0f;
    float StartDistance = 0.0f;
    float FogCutoffDistance = 1000.0f;
    float FogMaxOpacity = 0.0f;

    FLinearColor FogInscatteringColor{ 0.5,0.5,0.5,1.0 };
};

    