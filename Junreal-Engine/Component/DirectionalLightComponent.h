#pragma once
#include "Component/LightComponent.h"

class UDirectionalLightComponent : public ULightComponent
{
public:
	DECLARE_CLASS(UDirectionalLightComponent, ULightComponent)
	UDirectionalLightComponent();
	~UDirectionalLightComponent();

	UObject* Duplicate() override;

protected:
	void DuplicateSubObjects() override;
	void CopyCommonProperties(UObject* InTarget) override;

private:
	
};