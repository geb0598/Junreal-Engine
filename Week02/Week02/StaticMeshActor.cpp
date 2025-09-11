#include "StaticMeshActor.h"
#include "ObjectFactory.h"

AStaticMeshActor::AStaticMeshActor()
{
    Name = "Static Mesh Actor";
    StaticMeshComponent = NewObject<UStaticMeshComponent>();
    AddComponent(StaticMeshComponent);
    StaticMeshComponent->SetupAttachment(RootComponent);
}

AStaticMeshActor::~AStaticMeshActor()
{
    if (StaticMeshComponent)
    {
        ObjectFactory::DeleteObject(StaticMeshComponent);
    }
    StaticMeshComponent = nullptr;
}

void AStaticMeshActor::SetStaticMeshComponent(UStaticMeshComponent* InStaticMeshComponent)
{
    StaticMeshComponent = InStaticMeshComponent;
}
