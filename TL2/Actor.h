#pragma once
#include "Enums.h"
#include "Object.h"
#include "Vector.h"

class UWorld;
class USceneComponent;
class UAABoundingBoxComponent;
class UShapeComponent;

class AActor : public UObject
{
public:
    DECLARE_CLASS(AActor, UObject)
    AActor(); 

protected:
    ~AActor() override;

public:
    virtual void BeginPlay();
    virtual void Tick(float DeltaSeconds);
    virtual void EndPlay(const EEndPlayReason::Type EndPlayReason);

    virtual void Destroy();

    // ───────────────
    // Transform API
    // ───────────────
    void SetActorTransform(const FTransform& InNewTransform) const;
    FTransform GetActorTransform() const;   

    void SetActorLocation(const FVector& InNewLocation);
    FVector GetActorLocation() const;

    void SetActorRotation(const FVector& InEulerDegree) const;
    void SetActorRotation(const FQuat& InQuat) const;
    FQuat GetActorRotation() const;

    void SetActorScale(const FVector& InNewScale) const;
    FVector GetActorScale() const;

    FMatrix GetWorldMatrix() const;

    FVector GetActorForward() const { return GetActorRotation().RotateVector(FVector(0, 1, 0)); }
    FVector GetActorRight()   const { return GetActorRotation().RotateVector(FVector(1, 0, 0)); }
    FVector GetActorUp()      const { return GetActorRotation().RotateVector(FVector(0, 0, 1)); }

    void AddActorWorldRotation(const FQuat& InDeltaRotation) const;
    void AddActorWorldRotation(const FVector& DeltaEuler);
    void AddActorWorldLocation(const FVector& DeltaRot) const;

    void AddActorLocalRotation(const FVector& DeltaEuler);

    void AddActorLocalRotation(const FQuat& InDeltaRotation) const;
    void AddActorLocalLocation(const FVector& DeltaRot) const;

    USceneComponent* GetRootComponent() { return RootComponent; }

    void SetIsPicked(bool picked) { bIsPicked = picked; }
    bool GetIsPicked() { return bIsPicked; }



    //-----------------------------
    //----------Getter------------
    const TSet<USceneComponent*>& GetComponents() const;

    void SetName(const FString& InName) { Name = InName; }
    const FName& GetName() const { return Name; }

    template<typename T>
    T* CreateDefaultSubobject(const FName& SubobjectName)
    {
        // NewObject를 통해 생성
        T* Comp = ObjectFactory::NewObject<T>();
        Comp->SetOwner(this);
       // Comp->SetName(SubobjectName);  //나중에 추가 구현
        AddComponent(Comp);
        return Comp;
    }

    // Duplicate function
    UObject* Duplicate() override;
    void DuplicateSubObjects() override;

public:
    FName Name;
    USceneComponent* RootComponent = nullptr;
    
    UAABoundingBoxComponent* CollisionComponent = nullptr;

    // TODO(KHJ): Level->GetWorld();
    UWorld* World = nullptr;

    // Visibility properties
    void SetActorHiddenInGame(bool bNewHidden) { bHiddenInGame = bNewHidden; }
    bool GetActorHiddenInGame() const { return bHiddenInGame; }
    bool IsActorVisible() const { return !bHiddenInGame; }
    void AddComponent(USceneComponent* InComponent);

    UWorld* GetWorld() const override final;
    // TODO(KHJ): 제거 필요
    void SetWorld(UWorld* InWorld) { World = InWorld; }
    
protected:
    TSet<USceneComponent*> OwnedComponents;
    bool bIsPicked = false;
    bool bCanEverTick = true;
    bool bHiddenInGame = false;
    bool bTickInEditor = false;
};
