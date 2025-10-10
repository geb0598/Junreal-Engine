// DecalActor.cpp
#include "pch.h"
#include "DecalActor.h"
#include "OBoundingBoxComponent.h"

ADecalActor::ADecalActor()
{
    // DecalComponent 생성 및 루트로 설정
    DecalComponent = CreateDefaultSubobject<UDecalComponent>(FName("DecalComponent"));
    DecalComponent->SetupAttachment(RootComponent);

    // OBB 충돌 컴포넌트 생성 및 루트에 부착
    OBBCollisionComponent = CreateDefaultSubobject<UOBoundingBoxComponent>(FName("OBBCollisionBox"));
    OBBCollisionComponent->SetupAttachment(RootComponent);
}

ADecalActor::~ADecalActor()
{
}

void ADecalActor::Tick(float DeltaTime)
{
    AActor::Tick(DeltaTime);
    if(OBBCollisionComponent && DecalComponent && DecalComponent->GetDecalBoxMesh())
    {
        auto& vertices = DecalComponent->GetDecalBoxMesh()->GetStaticMeshAsset()->Vertices;
        std::vector<FVector> verts;
        for (const auto& v : vertices)
            verts.push_back(v.pos);
        OBBCollisionComponent->SetFromVertices(verts);
    }
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
    if (ComponentToDelete == OBBCollisionComponent)
    {
        OBBCollisionComponent = nullptr;
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

    // OBBCollisionComponent 찾기
    for (UActorComponent* Comp : OwnedComponents)
    {
        if (UOBoundingBoxComponent* OBBComp = Cast<UOBoundingBoxComponent>(Comp))
        {
            OBBCollisionComponent = OBBComp;
            break;
        }
    }
}
