#include "pch.h"
#include "GizmoArrowComponent.h"

UGizmoArrowComponent::UGizmoArrowComponent()
{
    SetMeshResource("Arrow.obj");
    SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoArrowComponent::~UGizmoArrowComponent()
{

}
