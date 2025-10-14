#pragma once
#include "SceneComponent.h"

class UMovementComponent : public UActorComponent
{
public:
	DECLARE_CLASS(UMovementComponent, UActorComponent);
	UMovementComponent();

	void TickComponent(float DeltaSeconds) override;
	void SetUpdatedComponent(USceneComponent* Component);

	void OnRegister() override;

protected:
	USceneComponent* UpdatedComponent;
	
private:
	FVector Velocity;
};
