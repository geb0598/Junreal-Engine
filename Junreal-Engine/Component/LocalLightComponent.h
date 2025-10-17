#pragma once
#include "Component/LightComponent.h"
class ULocalLightComponent : public ULigthComponent
{
public:
	DECLARE_CLASS(ULocalLightComponent, ULigthComponent)
	ULocalLightComponent();
	~ULocalLightComponent();

	const float GetAttenuationRadius() const;

	void SetAttenuationRadius(float InAttenuationRadius);

	UObject* Duplicate() override;
	void DuplicateSubObjects() override;
private:
	float AttenuationRadius;
};

