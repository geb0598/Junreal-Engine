#include "pch.h"
#include "Component/DirectionalLightComponent.h"

UDirectionalLightComponent::UDirectionalLightComponent()
{
}

UDirectionalLightComponent::~UDirectionalLightComponent()
{
}

UObject* UDirectionalLightComponent::Duplicate()
{
    UDirectionalLightComponent* DuplicatedComponent = Cast<UDirectionalLightComponent>(NewObject(GetClass()));
    if (DuplicatedComponent)
    {
        // 공통 속성 복제
        CopyCommonProperties(DuplicatedComponent);
        // 자식 컴포넌트 복제
        DuplicatedComponent->DuplicateSubObjects();
    }
    return DuplicatedComponent;
}

void UDirectionalLightComponent::DuplicateSubObjects()
{
    Super_t::DuplicateSubObjects();
}

void UDirectionalLightComponent::CopyCommonProperties(UObject* InTarget)
{
    Super_t::CopyCommonProperties(InTarget);
}
