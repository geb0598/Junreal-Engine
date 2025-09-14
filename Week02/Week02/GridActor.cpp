#include "pch.h"
#include "GridActor.h"

AGridActor::AGridActor()
{
    GroundGrid = NewObject<UStaticMeshComponent>();
    AxisGrid = NewObject<UStaticMeshComponent>();
    GroundGrid->SetupAttachment(RootComponent);
    AxisGrid->SetupAttachment(RootComponent);
    AddComponent(GroundGrid) ;
    AddComponent(AxisGrid);

    GroundGrid->SetMeshResource("Grid");
    GroundGrid->SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
    AxisGrid->SetMeshResource("Axis");
    AxisGrid->SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

AGridActor::~AGridActor()
{

}

