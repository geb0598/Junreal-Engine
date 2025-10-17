#include "Component/LightComponentBase.h"

ULightComponentBase::ULightComponentBase()
	: Intensity(10.0f), LightColor({1.0f, 1.0f, 0.0f, 1.0f}), bVisible(true)
{

}

ULightComponentBase::~ULightComponentBase()
{

}

const float ULightComponentBase::GetIntensity() const
{
	return Intensity;
}

const FColor& ULightComponentBase::GetLightColor() const
{
	return LightColor;
}

const bool ULightComponentBase::GetVisible() const
{
	return bVisible;
}

void ULightComponentBase::SetIntensity(float InIntensity)
{
	Intensity = InIntensity;
}

void ULightComponentBase::SetLightColor(const FColor& InLightColor)
{
	LightColor = InLightColor;
}

void ULightComponentBase::SetVisible(bool bInVisible)
{
	bVisible = bInVisible;
}

UObject* ULightComponentBase::Duplicate()
{
    ULightComponentBase* DuplicatedComponent = Cast<ULightComponentBase>(NewObject(GetClass()));
    if (DuplicatedComponent)
    {
        CopyCommonProperties(DuplicatedComponent);

        DuplicatedComponent->Intensity = Intensity;
        DuplicatedComponent->LightColor = LightColor;
        DuplicatedComponent->bVisible = bVisible;

        // 자식 컴포넌트 복제
        DuplicatedComponent->DuplicateSubObjects();
    }
    return DuplicatedComponent;
}

void ULightComponentBase::DuplicateSubObjects()
{
    Super_t::DuplicateSubObjects();
}
