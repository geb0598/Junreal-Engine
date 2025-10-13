#include "pch.h"
#include "ExponentialHeightFog.h"
#include "ExponentialHeightFogComponent.h"

AExponentialHeightFog::AExponentialHeightFog()
{
	HeightFogComponent = CreateDefaultSubobject<UExponentialHeightFogComponent>(FName("HeightFogComponent"));
    RootComponent = HeightFogComponent;

	SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(FName("SpriteComponent"));
    if (SpriteComponent)
    {
        SpriteComponent->SetTexture(FString("Editor/Icon/S_AtmosphericHeightFog.dds"));
        SpriteComponent->SetRelativeLocation(RootComponent->GetWorldLocation());
        SpriteComponent->SetEditable(false);
        SpriteComponent->SetupAttachment(RootComponent);
    }

}
