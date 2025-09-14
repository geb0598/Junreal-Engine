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

    GroundGrid->SetMesh("Grid");
    GroundGrid->SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
    AxisGrid->SetMesh("Axis");
    AxisGrid->SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

AGridActor::~AGridActor()
{

}

