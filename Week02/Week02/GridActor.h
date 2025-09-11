#pragma once
#include "Actor.h"
#include "StaticMeshComponent.h"
class AGridActor : public AActor
{
public:
    DECLARE_CLASS(AGridActor, AActor);
    AGridActor();

protected:
    ~AGridActor() override;

    UStaticMeshComponent* GroundGrid;
    UStaticMeshComponent* AxisGrid;
};

