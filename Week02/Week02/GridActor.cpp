#include "pch.h"
#include "GridActor.h"

AGridActor::AGridActor()
{
    GroundGrid = NewObject<UStaticMeshComponent>();
    AxisGrid = NewObject<UStaticMeshComponent>();
    GroundGrid->SetupAttachment(RootComponent);
    AxisGrid->SetupAttachment(RootComponent);
    AddComponent(GroundGrid);
    AddComponent(AxisGrid);

    GroundGrid->SetStaticMesh("Grid");
    AxisGrid->SetStaticMesh("Axis");
}

AGridActor::~AGridActor()
{

}

