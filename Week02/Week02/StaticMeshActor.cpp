#include "pch.h"
#include "BoundingBoxComponent.h"
#include "StaticMeshActor.h"
#include "ObjectFactory.h"

AStaticMeshActor::AStaticMeshActor()
{
    Name = "Static Mesh Actor";
    StaticMeshComponent = NewObject<UStaticMeshComponent>();
    AddComponent(StaticMeshComponent);
    StaticMeshComponent->SetupAttachment(RootComponent);
	CollisionComponent->SetupAttachment(RootComponent);

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

void AStaticMeshActor::SetCollisionComponent()
{
    if (UMeshComponent* MeshComp = Cast<UMeshComponent>(StaticMeshComponent)) {
        MeshComp->GetMeshResource()->GetMeshData();
        CollisionComponent->SetFromVertices(MeshComp->GetMeshResource()->GetMeshData()->Vertices);
    }
}
