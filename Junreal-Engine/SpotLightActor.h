#pragma once
#include "Actor.h"
#include "BillboardComponent.h"
#include "SpotLightComponent.h"

class ASpotLightActor : public AActor
{
public:
	DECLARE_CLASS(ASpotLightActor, AActor)

	ASpotLightActor();
	virtual ~ASpotLightActor() override;

	void Tick(float DeltaTime) override;

	UObject* Duplicate() override;
	void DuplicateSubObjects() override;

	USpotLightComponent* GetSpotLightComponent() const { return SpotLightComponent; }
	UBillboardComponent* GetSpriteComponent() const { return SpriteComponent; }

protected:
	UBillboardComponent* SpriteComponent;
	USpotLightComponent* SpotLightComponent;
};
