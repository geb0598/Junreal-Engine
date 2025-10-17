#include "Component/LocalLightComponent.h"
class UPointLightComponent : public ULocalLightComponent
{
public:
	DECLARE_CLASS(UPointLightComponent, ULocalLightComponent)
	UPointLightComponent();
	~UPointLightComponent();

	const float GetLightFalloffExponent() const;

	void SetLightFalloffExponent(float InLightFalloffExponent);

	UObject* Duplicate() override;
	void DuplicateSubObjects() override;
	void CopyCommonProperties(UObject* InTarget) override;
private:

	float LightFalloffExponent;
};