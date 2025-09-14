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

    GroundGrid->SetStaticMesh("Grid");
    GroundGrid->SetMesh("Grid");
    GroundGrid->SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
    AxisGrid->SetStaticMesh("Axis");
    AxisGrid->SetMesh("Axis");
    AxisGrid->SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

AGridActor::~AGridActor()
{

}

