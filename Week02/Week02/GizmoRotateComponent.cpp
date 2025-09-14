#include "pch.h"
#include "GizmoRotateComponent.h"

UGizmoRotateComponent::UGizmoRotateComponent()
{
    SetMeshResource("RotationHandle.obj");
   // SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
    SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoRotateComponent::~UGizmoRotateComponent()
{
}
