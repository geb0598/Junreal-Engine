#pragma once
#include "MovementComponent.h"

class UProjectileMovementComponent : public UMovementComponent
{
public:
	DECLARE_CLASS(UProjectileMovementComponent, UMovementComponent)
	UProjectileMovementComponent();

	void TickComponent(float DeltaSeconds) override;

private:
	float InitialSpeed;
	float MaxSpeed;
	float GravityScale;
};
