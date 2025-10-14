#pragma once
#include "MovementComponent.h"

class URotationMovementComponent : public UMovementComponent
{
public:
	DECLARE_CLASS(URotationMovementComponent, UMovementComponent)
	URotationMovementComponent();

	void TickComponent(float DeltaSeconds) override;

	// Getter/Setter Func
	FVector GetRotationRate() const { return RotationRate; }
	FVector GetPivotTranslation() const { return PivotTranslation; }
	void SetRotationRate(const FVector& InRotationRate) { RotationRate = InRotationRate; }
	void SetPivotTranslation(const FVector& InPivotTranslation) { PivotTranslation = InPivotTranslation; }

private:
	FVector RotationRate;
	FVector PivotTranslation;
	bool bRotationInLocalSpace;
};