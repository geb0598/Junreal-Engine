#pragma once
#include "Component/LightComponentBase.h"

class ULightComponent : public ULightComponentBase 
{
public:
	DECLARE_CLASS(ULightComponent, ULightComponentBase)
	
	ULightComponent() = default;
	~ULightComponent() = default;

	UObject* Duplicate() override;
	void DuplicateSubObjects() override;
	void CopyCommonProperties(UObject* InTarget) override;
private:

};