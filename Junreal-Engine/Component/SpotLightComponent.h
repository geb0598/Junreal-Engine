// SpotLightComponent.h (Inheritance Version)
#pragma once
#include "Component/PointLightComponent.h"

class USpotLightComponent : public UPointLightComponent
{
public:
    DECLARE_CLASS(USpotLightComponent, UPointLightComponent)

    USpotLightComponent();
    ~USpotLightComponent();

    const float GetInnerConeAngle() const;
    const float GetOuterConeAngle() const;

    void SetInnerConeAngle(float InInnerConeAngle);
    void SetOuterConeAngle(float InOuterConeAngle);

    UObject* Duplicate() override;
protected:
    void DuplicateSubObjects() override;
    void CopyCommonProperties(UObject* InTarget) override;
private:
    float InnerConeAngle;
    float OuterConeAngle;
};