#include "pch.h"
#include "SpotLightActor.h"
#include "ObjectFactory.h"

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
    SpotLightComponent->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 89.5, 0)));
    SetActorRotation(FVector(0, 90, 0));//빅이슈!!!!!!!!!!!!!!!!!!!!!
}

ASpotLightActor::~ASpotLightActor()
{
}
void ASpotLightActor::Tick(float DeltaTime)
{
    SpriteComponent->SetTexture(FString("Editor/Icon/SpotLight_64x.dds"));
}

UObject* ASpotLightActor::Duplicate()
{
    ASpotLightActor* NewActor = static_cast<ASpotLightActor*>(AActor::Duplicate());
    if (NewActor)
    {
        // SpotLightComponent는 부모의 Duplicate에서 처리됨
        NewActor->SpotLightComponent = static_cast<USpotLightComponent*>(NewActor->GetRootComponent());
        NewActor->SpriteComponent = static_cast<UBillboardComponent*>(NewActor->GetRootComponent());
    }
    return NewActor;
}

void ASpotLightActor::DuplicateSubObjects()
{
    AActor::DuplicateSubObjects();

    // SpotLightComponent 재설정
    SpotLightComponent = Cast<USpotLightComponent>(RootComponent);
}
