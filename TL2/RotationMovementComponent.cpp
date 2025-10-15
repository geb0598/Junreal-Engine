#include "pch.h"
#include "RotationMovementComponent.h"

URotationMovementComponent::URotationMovementComponent()
	: RotationRate(FVector(0.f, 20.f, 0.f))
	, PivotTranslation(FVector(0.f, 0.f, 0.f))
	, bRotationInLocalSpace(true)
{
	bCanEverTick = true;
	//bCanEverTick = true;
	WorldTickMode = EComponentWorldTickMode::PIEOnly; // PIE에서만 작동
}

void URotationMovementComponent::TickComponent(float DeltaSeconds)
{
	UActorComponent::TickComponent(DeltaSeconds);
	if (!UpdatedComponent)
	{
		return;
	}

	// Rotations Per Second
	const FVector DeltaRotation = RotationRate * DeltaSeconds;
	const FQuat DeltaQuat = FQuat::MakeFromEuler(DeltaRotation);

	// Apply Rotation
	if (bRotationInLocalSpace)
	{
		UpdatedComponent->AddLocalRotation(DeltaQuat);
	}
	else
	{
		UpdatedComponent->AddWorldRotation(DeltaQuat);
	}
}
