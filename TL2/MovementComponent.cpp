#include "pch.h"
#include "MovementComponent.h"

UMovementComponent::UMovementComponent()
	: UpdatedComponent(nullptr)
	, Velocity(FVector(0.f, 5.f, 0.f))
{
	bCanEverTick = true;
}

void UMovementComponent::TickComponent(float DeltaSeconds)
{
	UE_LOG("this actor is moving by movement component~");
	if (UpdatedComponent == nullptr || Velocity.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		return;
	}

	const FVector DeltaMovement = Velocity * DeltaSeconds;

	if (DeltaMovement.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		UpdatedComponent->AddWorldOffset(DeltaMovement);
	}
}

void UMovementComponent::SetUpdatedComponent(USceneComponent* Component)
{
	UpdatedComponent = Component;
}

void UMovementComponent::OnRegister()
{
	if (UpdatedComponent == nullptr)
	{
		AActor* Owner = GetOwner();
		if (Owner)
		{
			SetUpdatedComponent(Owner->GetRootComponent());
		}
	}
}
