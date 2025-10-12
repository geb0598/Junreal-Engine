#pragma once
#include "Actor.h"
#include "DecalComponent.h"

class ASpotLightActor : public AActor
{
public:
	DECLARE_CLASS(ASpotLightActor, AActor)

	ASpotLightActor();
	virtual ~ASpotLightActor() override;

protected:
	UDecalComponent* SpotLightComponent;
};
