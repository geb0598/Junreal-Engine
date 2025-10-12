// DecalActor.cpp
#include "pch.h"
#include "DecalActor.h"
#include "ObjectFactory.h"

ADecalActor::ADecalActor()
{
    // 기존 RootComponent 제거 후 DecalComponent를 Root로 설정
    if (RootComponent)
    {
        USceneComponent* TempRootComponent = RootComponent;
        RootComponent = nullptr;
        DeleteComponent(TempRootComponent);
    }

    DecalComponent = CreateDefaultSubobject<UDecalComponent>(FName("DecalComponent"));
    RootComponent = DecalComponent;
    AddComponent(DecalComponent);
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
    // 원본(this)의 컴포넌트들 저장
    USceneComponent* OriginalRoot = this->RootComponent;

    // 얕은 복사 수행 (생성자 실행됨 - DecalComponent 생성)
    ADecalActor* DuplicatedActor = NewObject<ADecalActor>(*this);

    // 생성자가 만든 컴포넌트 삭제
    if (DuplicatedActor->DecalComponent)
    {
        DuplicatedActor->OwnedComponents.Remove(DuplicatedActor->DecalComponent);
        ObjectFactory::DeleteObject(DuplicatedActor->DecalComponent);
        DuplicatedActor->DecalComponent = nullptr;
    }

    DuplicatedActor->RootComponent = nullptr;
    DuplicatedActor->OwnedComponents.clear();

    // 원본의 RootComponent(DecalComponent) 복제
    if (OriginalRoot)
    {
        DuplicatedActor->RootComponent = Cast<USceneComponent>(OriginalRoot->Duplicate());
    }

    // OwnedComponents 재구성 및 타입별 포인터 재설정
    DuplicatedActor->DuplicateSubObjects();

    return DuplicatedActor;
}

void ADecalActor::DuplicateSubObjects()
{
    // Duplicate()에서 이미 RootComponent를 복제했으므로
    // 부모 클래스가 OwnedComponents를 재구성
    Super_t::DuplicateSubObjects();

    // 타입별 포인터 재설정
    DecalComponent = Cast<UDecalComponent>(RootComponent);
}
