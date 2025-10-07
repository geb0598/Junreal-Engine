// DecalActor.cpp
#include "pch.h"
#include "DecalActor.h"

IMPLEMENT_CLASS(ADecalActor)

ADecalActor::ADecalActor()
{
    // DecalComponent 생성 및 루트로 설정
    DecalComponent = CreateDefaultSubobject<UDecalComponent>(FName("DecalComponent"));

    DecalComponent->SetupAttachment(RootComponent);


    CollisionComponent = CreateDefaultSubobject<UAABoundingBoxComponent>(FName("CollisionBox"));
    CollisionComponent->SetupAttachment(RootComponent);
}

ADecalActor::~ADecalActor()
{
}

void ADecalActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
    if(CollisionComponent)
    CollisionComponent->SetFromVertices(DecalComponent->GetDecalBoxMesh()->GetStaticMeshAsset()->Vertices);
}

void ADecalActor::SetDecalComponent(UDecalComponent* InDecalComponent)
{
    DecalComponent = InDecalComponent;
    DecalComponent->SetupAttachment(RootComponent);
}

bool ADecalActor::DeleteComponent(USceneComponent* ComponentToDelete)
{
    if (ComponentToDelete == DecalComponent)
    {
        // 루트 컴포넌트는 삭제할 수 없음
        return false;
    }
    return AActor::DeleteComponent(ComponentToDelete);
}

UObject* ADecalActor::Duplicate()
{
    ADecalActor* NewActor = static_cast<ADecalActor*>(AActor::Duplicate());
    if (NewActor)
    {
        // DecalComponent는 부모의 Duplicate에서 처리됨
        NewActor->DecalComponent = static_cast<UDecalComponent*>(NewActor->GetRootComponent());
    }
    return NewActor;
}

void ADecalActor::DuplicateSubObjects()
{
    AActor::DuplicateSubObjects();
}
