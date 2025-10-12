#include "pch.h"
#include "SpotLightActor.h"

ASpotLightActor::ASpotLightActor()
{
    SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(FName("SpriteComponent"));
    if (SpriteComponent)
    {
        SpriteComponent->SetTexture(FString("Editor/Icon/SpotLight_64x.dds"));
        SpriteComponent->SetupAttachment(RootComponent);
        SpriteComponent->SetRelativeLocation(RootComponent->GetWorldLocation());
    }

    SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(FName("SpotLightComponent"));
    SpotLightComponent->SetupAttachment(RootComponent);
}

ASpotLightActor::~ASpotLightActor()
{
}
