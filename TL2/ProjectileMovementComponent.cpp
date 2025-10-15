#include "pch.h"
#include "ProjectileMovementComponent.h"

UProjectileMovementComponent::UProjectileMovementComponent()
	: InitialSpeed(10.0f)
	, MaxSpeed(100.f)
	, GravityScale(1.0f)
{
	bCanEverTick = true;
	WorldTickMode = EComponentWorldTickMode::PIEOnly; // PIE에서만 작동
	// Initial speed (forward)
	Velocity = FVector(0.f, InitialSpeed, 0.0f);
}

void UProjectileMovementComponent::TickComponent(float DeltaSeconds)
{
	UMovementComponent::TickComponent(DeltaSeconds);
	Velocity.Z -= 9.8f * GravityScale * DeltaSeconds;

	if (Velocity.Size() > MaxSpeed)
	{
		Velocity = Velocity.GetSafeNormal() * MaxSpeed;
	}

	FVector Delta = Velocity * DeltaSeconds;

	if (Delta.SizeSquared() > KINDA_SMALL_NUMBER)
	{
		UpdatedComponent->AddWorldOffset(Delta);
	}
}

UObject* UProjectileMovementComponent::Duplicate()
{
	UProjectileMovementComponent* Newcomp = Cast<UProjectileMovementComponent>(UMovementComponent::Duplicate());
	if (Newcomp)
	{
		Newcomp->InitialSpeed = InitialSpeed;
		Newcomp->MaxSpeed = MaxSpeed;
		Newcomp->GravityScale = GravityScale;
	}
	return Newcomp;
}

void UProjectileMovementComponent::DuplicateSubObjects()
{
	UMovementComponent::DuplicateSubObjects();
}