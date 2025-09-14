#include "pch.h"
#include "GizmoRotateComponent.h"

UGizmoRotateComponent::UGizmoRotateComponent()
{
    SetMesh("RotationHandle.obj");
    SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoRotateComponent::~UGizmoRotateComponent()
{
}
