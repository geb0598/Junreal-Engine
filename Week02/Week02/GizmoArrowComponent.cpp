#include "pch.h"
#include "GizmoArrowComponent.h"

UGizmoArrowComponent::UGizmoArrowComponent()
{
    SetStaticMesh("Arrow.obj");
    SetMeshResource("Arrow.obj");
    SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
}

UGizmoArrowComponent::~UGizmoArrowComponent()
{

}
