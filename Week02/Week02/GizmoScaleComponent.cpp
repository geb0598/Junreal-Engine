#include "pch.h"
#include "GizmoScaleComponent.h"

UGizmoScaleComponent::UGizmoScaleComponent()
{
    SetMesh("ScaleHandle.obj");
    SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoScaleComponent::~UGizmoScaleComponent()
{
}
