﻿#include "pch.h"
#include "GizmoArrowComponent.h"

UGizmoArrowComponent::UGizmoArrowComponent()
{
    SetStaticMesh("Data/Arrow.obj");
    SetMaterial("Primitive.hlsl");
}

UGizmoArrowComponent::~UGizmoArrowComponent()
{

}
