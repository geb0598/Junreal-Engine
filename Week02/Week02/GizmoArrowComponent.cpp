#include "pch.h"
#include "GizmoArrowComponent.h"

UGizmoArrowComponent::UGizmoArrowComponent()
{
    SetMesh("Arrow.obj");
    SetShader("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoArrowComponent::~UGizmoArrowComponent()
{

}
