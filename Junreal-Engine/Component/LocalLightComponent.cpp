#include "pch.h"
#include "Component/LocalLightComponent.h"

ULocalLightComponent::ULocalLightComponent()
	: AttenuationRadius(10.0f)
{
}

ULocalLightComponent::~ULocalLightComponent()
{
}

const float ULocalLightComponent::GetAttenuationRadius() const
{
	return AttenuationRadius;
}

void ULocalLightComponent::SetAttenuationRadius(float InAttenuationRadius)
{
	AttenuationRadius = InAttenuationRadius;
}

UObject* ULocalLightComponent::Duplicate()
{
    ULocalLightComponent* DuplicatedComponent = Cast<ULocalLightComponent>(NewObject(GetClass()));
    if (DuplicatedComponent)
    {
        CopyCommonProperties(DuplicatedComponent);

        DuplicatedComponent->AttenuationRadius = AttenuationRadius;

        // 자식 컴포넌트 복제
        DuplicatedComponent->DuplicateSubObjects();
    }
    return DuplicatedComponent;
}

void ULocalLightComponent::DuplicateSubObjects()
{
    Super_t::DuplicateSubObjects();
}
