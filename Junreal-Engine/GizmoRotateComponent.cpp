﻿#include "pch.h"
#include "GizmoRotateComponent.h"

UGizmoRotateComponent::UGizmoRotateComponent()
{
    SetStaticMesh("Data/RotationHandle.obj");
    SetMaterial("Primitive.hlsl");
}

UGizmoRotateComponent::~UGizmoRotateComponent()
{
}
