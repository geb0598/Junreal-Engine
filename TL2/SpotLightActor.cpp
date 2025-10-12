#include "pch.h"
#include "SpotLightActor.h"

ASpotLightActor::ASpotLightActor()
{
	SpotLightComponent = CreateDefaultSubobject<UDecalComponent>(FName("SpotLightComponent"));
	SpotLightComponent->SetupAttachment(RootComponent);
}

ASpotLightActor::~ASpotLightActor()
{
}
