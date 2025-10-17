#pragma once
#include "Component/LightComponent.h"
class UAmbientLightComponent : public ULightComponent
{
public:
	DECLARE_CLASS(UAmbientLightComponent, ULightComponent)

	UAmbientLightComponent() = default;
	~UAmbientLightComponent() = default;

	UObject* Duplicate() override;
	void DuplicateSubObjects() override;
	void CopyCommonProperties(UObject* InTarget) override;
private:

};