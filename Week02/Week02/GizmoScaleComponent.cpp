#include "pch.h"
#include "GizmoScaleComponent.h"

UGizmoScaleComponent::UGizmoScaleComponent()
{
    SetMeshResource("ScaleHandle.obj");
    SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoScaleComponent::~UGizmoScaleComponent()
{
}
