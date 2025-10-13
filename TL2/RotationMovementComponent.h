#pragma once
#include "MovementComponent.h"

class URotationMovementComponent : public UMovementComponent
{
public:
	DECLARE_CLASS(URotationMovementComponent, UMovementComponent)
	URotationMovementComponent();

	void TickComponent(float DeltaSeconds) override;

private:
	FVector RotationRate;
	FVector PivotTranslation;
	bool bRotationInLocalSpace;
};

