#pragma once
#include "SceneComponent.h"

class UExponentialHeightFogComponent : public USceneComponent
{

public:
    DECLARE_CLASS(UExponentialHeightFogComponent, USceneComponent);

    struct FFogInfo
    {
        float FogDensity;
        float FogHeightFalloff;
        float StartDistance;
        float FogCutoffDistance;
        float FogMaxOpacity;

        FLinearColor FogInscatteringColor;
    };

    void Render(URenderer* Renderer, const FVector& CameraPosition, const FMatrix& View, const FMatrix& Projection, FViewport* Viewport);
    FFogInfo GetFogInfo() const {
        return FFogInfo(
            FogDensity,
            FogHeightFalloff,
            StartDistance,
            FogCutoffDistance,
            FogMaxOpacity,
            FogInscatteringColor);
    }
    void SetFogInfo(const FFogInfo& Info){
        FogDensity = Info.FogDensity;
        FogHeightFalloff = Info.FogHeightFalloff;
        StartDistance = Info.StartDistance;
        FogCutoffDistance = Info.FogCutoffDistance;
        FogMaxOpacity = Info.FogMaxOpacity;
        FogInscatteringColor = Info.FogInscatteringColor;
    }
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

    