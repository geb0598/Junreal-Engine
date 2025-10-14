#include "pch.h"
#include "StaticMeshActor.h"
#include "ObjectFactory.h"

AStaticMeshActor::AStaticMeshActor()
{

    Name = "Static Mesh Actor";
    StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
    RootComponent = StaticMeshComponent;
}

void AStaticMeshActor::Tick(float DeltaTime)
{
    static float times;
    times += DeltaTime;
    //if (World->WorldType == EWorldType::PIE) {
    //    RootComponent->AddLocalRotation({ 0.01f, 0.0f,0.0f });
    //    RootComponent->AddLocalOffset({ sin(times)/100, sin(times)/100,sin(times)/100 });
    //}
    /*if(bIsPicked&& CollisionComponent)
    CollisionComponent->SetFromVertices(StaticMeshComponent->GetStaticMesh()->GetStaticMeshAsset()->Vertices);*/
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

UObject* AStaticMeshActor::Duplicate()
{
    // 원본(this)의 컴포넌트들 저장
    USceneComponent* OriginalRoot = this->RootComponent;

    // 얕은 복사 수행 (생성자 실행됨 - StaticMeshComponent, CollisionComponent 생성)
    AStaticMeshActor* DuplicatedActor = NewObject<AStaticMeshActor>(*this);

    // 생성자가 만든 컴포넌트들 삭제
    if (DuplicatedActor->StaticMeshComponent)
    {
        DuplicatedActor->OwnedComponents.Remove(DuplicatedActor->StaticMeshComponent);
        ObjectFactory::DeleteObject(DuplicatedActor->StaticMeshComponent);
        DuplicatedActor->StaticMeshComponent = nullptr;
    }

    DuplicatedActor->RootComponent = nullptr;
    DuplicatedActor->OwnedComponents.clear();

    // 원본의 RootComponent(StaticMeshComponent) 복제
    if (OriginalRoot)
    {
        DuplicatedActor->RootComponent = Cast<USceneComponent>(OriginalRoot->Duplicate());
    }

    // OwnedComponents 재구성 및 타입별 포인터 재설정
    DuplicatedActor->DuplicateSubObjects();

    return DuplicatedActor;
}

void AStaticMeshActor::DuplicateSubObjects()
{
    // Duplicate()에서 이미 RootComponent를 복제했으므로
    // 부모 클래스가 OwnedComponents를 재구성
    Super_t::DuplicateSubObjects();

    // 타입별 포인터 재설정
    StaticMeshComponent = Cast<UStaticMeshComponent>(RootComponent);
}

// 특화된 멤버 컴포넌트 CollisionComponent, StaticMeshComponent 는 삭제 시 포인터를 초기화합니다.
bool AStaticMeshActor::DeleteComponent(UActorComponent* ComponentToDelete)
{
    // 2. [부모 클래스의 원래 기능 호출]
    // 기본적인 삭제 로직(소유 목록 제거, 메모리 해제 등)은 부모에게 위임합니다.
    // Super:: 키워드를 사용합니다.
    return AActor::DeleteComponent(ComponentToDelete);
}
