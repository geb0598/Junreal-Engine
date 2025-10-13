#include "pch.h"
#include "MovementComponent.h"

UMovementComponent::UMovementComponent()
	: UpdatedComponent(nullptr)
	, Velocity(FVector(0.f, 0.f, 0.f))
{
}

void UMovementComponent::TickComponent(float DeltaSeconds)
{
}

void UMovementComponent::SetUpdatedComponent(USceneComponent* Component)
{
	UpdatedComponent = Component;
}
