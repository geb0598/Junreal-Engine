// SpotLightComponent.cpp (Inheritance Version)
#include "pch.h"
#include "Component/SpotLightComponent.h"

USpotLightComponent::USpotLightComponent()
	: InnerConeAngle(30), OuterConeAngle(60)
{

}

USpotLightComponent::~USpotLightComponent()
{
}

const float USpotLightComponent::GetInnerConeAngle() const
{
	return InnerConeAngle;
}

const float USpotLightComponent::GetOuterConeAngle() const
{
	return OuterConeAngle;
}

void USpotLightComponent::SetInnerConeAngle(float InInnerConeAngle)
{
	InnerConeAngle = InInnerConeAngle;
}

void USpotLightComponent::SetOuterConeAngle(float InOuterConeAngle)
{
	OuterConeAngle = InOuterConeAngle;
}

UObject* USpotLightComponent::Duplicate()
{
	USpotLightComponent* DuplicatedComponent = Cast<USpotLightComponent>(NewObject(GetClass()));
	if (DuplicatedComponent)
	{
		// 공통 속성 복제
		CopyCommonProperties(DuplicatedComponent);
		// 자식 컴포넌트 복제
		DuplicatedComponent->DuplicateSubObjects();
	}
	return DuplicatedComponent;
}

void USpotLightComponent::DuplicateSubObjects()
{
	Super_t::DuplicateSubObjects();
}

void USpotLightComponent::CopyCommonProperties(UObject* InTarget)
{
	Super_t::CopyCommonProperties(InTarget);

	USpotLightComponent* Target = Cast<USpotLightComponent>(InTarget);
	if (Target)
	{
		Target->InnerConeAngle = InnerConeAngle;
		Target->OuterConeAngle = OuterConeAngle;
	}
}
