#pragma once
#include "Object.h"
#include "Vector.h"

class UWorld;
class USceneComponent;
class USceneComponent;

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
    virtual void Destroy();

    // ───────────────
    // Transform API
    // ───────────────
    void SetActorTransform(const FTransform& NewTransform);
    FTransform GetActorTransform() const;

    void SetActorLocation(const FVector& NewLocation);
    FVector GetActorLocation() const;

    void SetActorRotation(const FVector& EulerDegree);
    void SetActorRotation(const FQuat& InQuat);
    FQuat GetActorRotation() const;

    void SetActorScale(const FVector& NewScale);
    FVector GetActorScale() const;

    FMatrix GetWorldMatrix() const;

    FVector GetActorForward() const { return GetActorRotation().RotateVector(FVector(0, 1, 0)); }
    FVector GetActorRight()   const { return GetActorRotation().RotateVector(FVector(1, 0, 0)); }
    FVector GetActorUp()      const { return GetActorRotation().RotateVector(FVector(0, 0, 1)); }

    void AddActorWorldRotation(const FQuat& DeltaRotation);
    void AddActorWorldRotation(const FVector& DeltaEuler);
    void AddActorWorldLocation(const FVector& DeltaRot);

    void AddActorLocalRotation(const FVector& DeltaEuler);

    void AddActorLocalRotation(const FQuat& DeltaRotation);
    void AddActorLocalLocation(const FVector& DeltaRot);

    void SetWorld(UWorld* InWorld) { World = InWorld; }




    //-----------------------------
    //----------Getter------------
    const TArray<USceneComponent*>& GetComponents() const;

    void SetName(const FString& InName) { Name = InName; }
    const FString& GetName() { return Name; }

public:
    FString Name;
    USceneComponent* RootComponent = nullptr;

    UWorld* World = nullptr;
protected:
    TArray<USceneComponent*> Components;
    bool bCanEverTick = true;
    void AddComponent(USceneComponent* Component);
};
