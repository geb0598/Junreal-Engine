#include "pch.h"
#include "GizmoScaleComponent.h"

UGizmoScaleComponent::UGizmoScaleComponent()
{
    SetStaticMesh("ScaleHandle.obj");
    SetMesh("ScaleHandle.obj");
    SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoScaleComponent::~UGizmoScaleComponent()
{
}
