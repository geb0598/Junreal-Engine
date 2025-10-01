#include "pch.h"
#include "Actor.h"
#include "SceneComponent.h"
#include "ObjectFactory.h"
#include "ShapeComponent.h"
#include "AABoundingBoxComponent.h"
#include "MeshComponent.h"
#include "TextRenderComponent.h"

AActor::AActor()
{
    Name = "DefaultActor";
    RootComponent = CreateDefaultSubobject<USceneComponent>(FName("SceneComponent"));
}

AActor::~AActor()
{
    for (USceneComponent* Comp : OwnedComponents)
    {
        if (Comp)
        {
            ObjectFactory::DeleteObject(Comp);
        }
    }
    OwnedComponents.Empty();
}

void AActor::BeginPlay()
{
}

void AActor::Tick(float DeltaSeconds)
{
    // 소유한 모든 컴포넌트의 Tick 처리
    for (UActorComponent* Component : OwnedComponents)
    {
        if (Component && Component->CanEverTick())
        {
            Component->TickComponent(DeltaSeconds);
        }
    }
}

/**
 * @brief Endplay 전파 함수
 * @param EndPlayReason Endplay 이유, Type에 따른 다른 설정이 가능함
 */
void AActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
    for (USceneComponent* Component : OwnedComponents)
    {
        Component->EndPlay(EndPlayReason);
    }
}

void AActor::Destroy()
{
    if (!bCanEverTick) return;
    // Prefer world-managed destruction to remove from world actor list
    if (GetWorld())
    {
        // Avoid using 'this' after the call
        GetWorld()->DestroyActor(this);
        return;
    }
    // Fallback: directly delete the actor via factory
    ObjectFactory::DeleteObject(this);
}

// ───────────────
// Transform API
// ───────────────
void AActor::SetActorTransform(const FTransform& InNewTransform) const
{
    if (RootComponent)
    {
        RootComponent->SetWorldTransform(InNewTransform);
    }
}


FTransform AActor::GetActorTransform() const
{
    return RootComponent ? RootComponent->GetWorldTransform() : FTransform();
}

void AActor::SetActorLocation(const FVector& InNewLocation)
{
    if (RootComponent)
    {
        RootComponent->SetWorldLocation(InNewLocation);
    }
}

FVector AActor::GetActorLocation() const
{
    return RootComponent ? RootComponent->GetWorldLocation() : FVector();
}

void AActor::SetActorRotation(const FVector& InEulerDegree) const
{
    if (RootComponent)
    {
        RootComponent->SetWorldRotation(FQuat::MakeFromEuler(InEulerDegree));
    }
}

void AActor::SetActorRotation(const FQuat& InQuat) const
{
    if (RootComponent)
    {
        RootComponent->SetWorldRotation(InQuat);
    }
}

FQuat AActor::GetActorRotation() const
{
    return RootComponent ? RootComponent->GetWorldRotation() : FQuat();
}

void AActor::SetActorScale(const FVector& InNewScale) const
{
    if (RootComponent)
    {
        RootComponent->SetWorldScale(InNewScale);
    }
}

FVector AActor::GetActorScale() const
{
    return RootComponent ? RootComponent->GetWorldScale() : FVector(1, 1, 1);
}

FMatrix AActor::GetWorldMatrix() const
{
    return RootComponent ? RootComponent->GetWorldMatrix() : FMatrix::Identity();
}

void AActor::AddActorWorldRotation(const FQuat& InDeltaRotation) const
{
    if (RootComponent)
    {
        RootComponent->AddWorldRotation(InDeltaRotation);
    }
}

void AActor::AddActorWorldLocation(const FVector& InDeltaLocation) const
{
    if (RootComponent)
    {
        RootComponent->AddWorldOffset(InDeltaLocation);
    }
}

void AActor::AddActorLocalRotation(const FQuat& InDeltaRotation) const
{
    if (RootComponent)
    {
        RootComponent->AddLocalRotation(InDeltaRotation);
    }
}

void AActor::AddActorLocalLocation(const FVector& InDeltaLocation) const
{
    if (RootComponent)
    {
        RootComponent->AddLocalOffset(InDeltaLocation);
    }
}

const TSet<USceneComponent*>& AActor::GetComponents() const
{
    return OwnedComponents;
}

void AActor::AddComponent(USceneComponent* InComponent)
{
    if (!InComponent)
    {
        return;
    }

    OwnedComponents.Add(InComponent);
    if (!RootComponent)
    {
        RootComponent = InComponent;
    }
}

UWorld* AActor::GetWorld() const
{
    // TODO(KHJ): Level 생기면 붙일 것
    // ULevel* Level = GetOuter();
    // if (Level)
    // {
    //     return Level->GetWorld();
    // }

    // return nullptr;

    return World;
}

/**
 * Actor의 복사 함수
 * @return 기본적으로 UObject의 복사함수랑 동일하나, 내부 함수를 템플릿 메서드 패턴에 따라 처리했음
 */
UObject* AActor::Duplicate()
{
    AActor* NewActor = new AActor(*this);
    NewActor->DuplicateSubObjects();

    return NewActor;
}

/**
 * @brief Actor의 Internal 복사 함수
 * 원본이 들고 있던 Component를 각 Component의 복사함수를 호출하여 받아온 후 새로 담아서 처리함
 */
void AActor::DuplicateSubObjects()
{
    TSet<USceneComponent*> DuplicatedComponents = OwnedComponents;
    OwnedComponents.Empty();

    for (USceneComponent* Component : DuplicatedComponents)
    {
        USceneComponent* NewComponent = static_cast<USceneComponent*>(Component->Duplicate());
        OwnedComponents.Add(NewComponent);
    }
}
