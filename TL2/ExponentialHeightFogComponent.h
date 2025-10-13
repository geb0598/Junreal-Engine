#pragma once
#include "SceneComponent.h"

class UExponentialHeightFogComponent : public USceneComponent
{
    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;

    FLinearColor FogInscatteringColor;
};

    