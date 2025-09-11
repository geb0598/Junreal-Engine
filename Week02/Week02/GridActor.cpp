#include "GridActor.h"

AGridActor::AGridActor()
{
    GroundGrid = new UStaticMeshComponent();
    AxisGrid = new UStaticMeshComponent();
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

