// SpotLightComponent.cpp (Inheritance Version)
#include "pch.h"
#include "SpotLightComponent.h"
#include "ResourceManager.h"

IMPLEMENT_CLASS(USpotLightComponent)

USpotLightComponent::USpotLightComponent()
{
    // The parent UDecalComponent constructor has already run and handled:
    // - Loading the default DecalBoxMesh
    // - Setting the default TexturePath
    // - Initializing LocalAABB, etc.

    // We just need to override the parts that are different for a spotlight.

    // 1. Use the new spotlight shader instead of the default decal shader.
    SetMaterial("SpotLightShader.hlsl");

    // 3. Call our overridden update function to create the initial perspective matrix.
    UpdateDecalProjectionMatrix();
}

void USpotLightComponent::UpdateDecalProjectionMatrix()
{
    FOBB WorldOBB = GetWorldOBB();
    float Right = WorldOBB.Extents.X;
    float Left = -WorldOBB.Extents.X;
    float Top = WorldOBB.Extents.Y;
    float Bottom = -WorldOBB.Extents.Y;
    float Near = 0.0f;
    float Far = WorldOBB.Extents.Z * 2;

    FMatrix OrthoMatrix = FMatrix::OffCenterOrthoLH(Left, Right, Top, Bottom, Near, Far);

    // UV 타일링을 위한 스케일 행렬 생성
    FMatrix UVScale = FMatrix::Identity();
    UVScale.M[0][0] = UVTiling.X;
    UVScale.M[1][1] = 1.0f;
    UVScale.M[2][2] = UVTiling.Y;
    // 중심 보정 (짝수일 때 반 픽셀 밀림 방지)
    // 중심 보정 - 타일링된 텍스처를 중앙에 배치
    UVScale.M[3][0] = -(UVTiling.X - 1.0f) / 2.0f;
    UVScale.M[3][1] = -(UVTiling.Y - 1.0f) / 2.0f;
    DecalProjectionMatrix = UVScale * OrthoMatrix;
}

// Note: All other methods like Tick, Render, Duplicate, SetDecalTexture, etc., 
// are automatically inherited from UDecalComponent. We don't need to rewrite them here.