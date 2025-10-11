// DecalActor.cpp
#include "pch.h"
#include "DecalActor.h"

ADecalActor::ADecalActor()
{
    // DecalComponent 생성 및 루트로 설정
    DecalComponent = CreateDefaultSubobject<UDecalComponent>(FName("DecalComponent"));
    DecalComponent->SetupAttachment(RootComponent);
}

ADecalActor::~ADecalActor()
{
}

void ADecalActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
    //if(OBBCollisionComponent && DecalComponent && DecalComponent->GetDecalBoxMesh())
    //{
    //    auto& vertices = DecalComponent->GetDecalBoxMesh()->GetStaticMeshAsset()->Vertices;
    //    std::vector<FVector> verts;
    //    for (const auto& v : vertices)
    //        verts.push_back(v.pos);
    //    OBBCollisionComponent->SetFromVertices(verts);
    //}
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

    // DecalComponent 재설정
    DecalComponent = Cast<UDecalComponent>(RootComponent);

}
