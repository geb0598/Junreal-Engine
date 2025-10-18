#include "pch.h"
#include "PointLightComponent.h"

UPointLightComponent::UPointLightComponent()
	: LightFalloffExponent(1.0f)
{
}

UPointLightComponent::~UPointLightComponent()
{
}

const float UPointLightComponent::GetLightFalloffExponent() const
{
	return LightFalloffExponent;
}

void UPointLightComponent::SetLightFalloffExponent(float InLightFalloffExponent)
{
	LightFalloffExponent = InLightFalloffExponent;
}

UObject* UPointLightComponent::Duplicate()
{
    UPointLightComponent* DuplicatedComponent = Cast<UPointLightComponent>(NewObject(GetClass()));
    if (DuplicatedComponent)
    {
        CopyCommonProperties(DuplicatedComponent);

        // 자식 컴포넌트 복제
        DuplicatedComponent->DuplicateSubObjects();
    }
    return DuplicatedComponent;
}

void UPointLightComponent::DuplicateSubObjects()
{
    Super_t::DuplicateSubObjects();
}

void UPointLightComponent::CopyCommonProperties(UObject* InTarget)
{
    // 부모 속성 복제
    Super_t::CopyCommonProperties(InTarget);

    // 자신의 속성 복제
    UPointLightComponent* Target = Cast<UPointLightComponent>(InTarget);
    if (Target)
    {
        Target->LightFalloffExponent = LightFalloffExponent;
    }
}
  