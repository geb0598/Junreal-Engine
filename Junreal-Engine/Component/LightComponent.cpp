#include "pch.h"
#include "Component/LightComponent.h"

UObject* ULightComponent::Duplicate()
{
    ULightComponent* DuplicatedComponent = Cast<ULightComponent>(NewObject(GetClass()));
    if (DuplicatedComponent)
    {
        // 공통 속성 복제
        CopyCommonProperties(DuplicatedComponent);
        // 자식 컴포넌트 복제
        DuplicatedComponent->DuplicateSubObjects();
    }
    return DuplicatedComponent;
}

void ULightComponent::DuplicateSubObjects()
{
    Super_t::DuplicateSubObjects();
}

void ULightComponent::CopyCommonProperties(UObject* InTarget)
{
    // 부모의 속성 복사
    Super_t::CopyCommonProperties(InTarget);
    
    // 복사할 자신의 속성 없으니 끝

}
