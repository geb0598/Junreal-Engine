#include "pch.h"
#include "FXAAActor.h"
#include "FXAAComponent.h"

AFXAAActor::AFXAAActor()
{
    FXAAComponent = CreateDefaultSubobject<UFXAAComponent>(FName("FXAAComponent"));
    RootComponent = FXAAComponent;

    SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(FName("SpriteComponent"));
    if (SpriteComponent)
    {
        SpriteComponent->SetTexture(FString("Editor/Icon/S_AtmosphericHeightFog.dds"));
        SpriteComponent->SetRelativeLocation(RootComponent->GetWorldLocation());
        SpriteComponent->SetEditable(false);
        SpriteComponent->SetupAttachment(RootComponent);
    }

}
