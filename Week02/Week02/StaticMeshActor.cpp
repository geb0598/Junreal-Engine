#include "pch.h"
#include "AABoundingBoxComponent.h"
#include "StaticMeshActor.h"
#include "ObjectFactory.h"

AStaticMeshActor::AStaticMeshActor()
{
    Name = "Static Mesh Actor";
    StaticMeshComponent = NewObject<UStaticMeshComponent>();
    AddComponent(StaticMeshComponent);
    StaticMeshComponent->SetupAttachment(RootComponent);

    CollisionComponent = CreateDefaultSubobject<UAABoundingBoxComponent>(FName("CollisionBox"));
    AddComponent(CollisionComponent);
	CollisionComponent->SetupAttachment(RootComponent);
}

void AStaticMeshActor::Tick(float DeltaTime)
{
    if (UMeshComponent* MeshComp = Cast<UMeshComponent>(StaticMeshComponent)) 
    {
        MeshComp->GetMeshResource()->GetMeshData();
        CollisionComponent->SetFromVertices(MeshComp->GetMeshResource()->GetMeshData()->Vertices);
    }
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

void AStaticMeshActor::SetCollisionComponent(EPrimitiveType InType)
{
    if (UMeshComponent* MeshComp = Cast<UMeshComponent>(StaticMeshComponent)) {
        MeshComp->GetMeshResource()->GetMeshData();
        CollisionComponent->SetFromVertices(MeshComp->GetMeshResource()->GetMeshData()->Vertices);
        CollisionComponent->SetPrimitiveType(InType);
    }
}
