#pragma once
#include "Component/LightComponent.h"
class ULocalLightComponent : public ULightComponent
{
public:
	DECLARE_CLASS(ULocalLightComponent, ULightComponent)
	ULocalLightComponent();
	~ULocalLightComponent();

	const float GetAttenuationRadius() const;

	void SetAttenuationRadius(float InAttenuationRadius);

	UObject* Duplicate() override;
	void DuplicateSubObjects() override;
	void CopyCommonProperties(UObject* InTarget) override;
private:
	float AttenuationRadius;
};

