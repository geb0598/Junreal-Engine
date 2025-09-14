#include "pch.h"
#include "GizmoRotateComponent.h"

UGizmoRotateComponent::UGizmoRotateComponent()
{
    SetStaticMesh("RotationHandle.obj");
    SetMesh("RotationHandle.obj");
    SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoRotateComponent::~UGizmoRotateComponent()
{
}
