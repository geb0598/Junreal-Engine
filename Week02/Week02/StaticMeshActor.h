#pragma once
#include "Actor.h"
#include "StaticMeshComponent.h"
#include "Enums.h"
class AStaticMeshActor : public AActor
{
public:
    DECLARE_CLASS(AStaticMeshActor, AActor)

    AStaticMeshActor();

protected:
    ~AStaticMeshActor() override;

public:
    UStaticMeshComponent* GetStaticMeshComponent() const { return StaticMeshComponent; }
    void SetStaticMeshComponent(UStaticMeshComponent* InStaticMeshComponent);
	void SetCollisionComponent();

protected:
    UStaticMeshComponent* StaticMeshComponent;
};

