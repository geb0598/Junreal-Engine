// SpotLightComponent.h (Inheritance Version)
#pragma once
#include "DecalComponent.h"

class USpotLightComponent : public UDecalComponent
{
public:
    DECLARE_CLASS(USpotLightComponent, UDecalComponent)

    USpotLightComponent();

    // Spotlight specific properties
    void SetSpotlightAngle(float Angle) { SpotlightAngle = Angle; UpdateDecalProjectionMatrix(); }
    float GetSpotlightAngle() const { return SpotlightAngle; }
    void SetAspectRatio(float Ratio) { AspectRatio = Ratio; UpdateDecalProjectionMatrix(); }
    float GetAspectRatio() const { return AspectRatio; }

protected:
    void UpdateDecalProjectionMatrix() override;

    // Spotlight-specific properties
    float SpotlightAngle = 45.0f; // degrees
    float AspectRatio = 1.0f;
    
    // Note: All other variables like DecalBoxMesh, TexturePath, Material, etc.,
    // are now inherited from UDecalComponent and don't need to be redeclared.
};