#include "pch.h"
#include "GizmoScaleComponent.h"

UGizmoScaleComponent::UGizmoScaleComponent()
{
    SetStaticMesh("ScaleHandle.obj");
    SetMeshResource("ScaleHandle.obj");
    SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoScaleComponent::~UGizmoScaleComponent()
{
}
