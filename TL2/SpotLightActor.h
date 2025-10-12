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

protected:
	UBillboardComponent* SpriteComponent;
	USpotLightComponent* SpotLightComponent;
};
