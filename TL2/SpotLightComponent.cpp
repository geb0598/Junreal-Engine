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

    // 1. Calculate and update member variables from the OBB extents
    Radius = WorldOBB.Extents.X;
    Height = WorldOBB.Extents.Y;
    Near = 1e-2f;
    Far = Height * 1.1f;

    if (Height < KINDA_SMALL_NUMBER) return;

    Fov = atanf(Radius / Height) * 2.0f; // radians
    Aspect = (WorldOBB.Extents.Y > KINDA_SMALL_NUMBER) ? WorldOBB.Extents.X / WorldOBB.Extents.Y : 1.0f;

    // 2. Create the projection matrix using the member variables
    DecalProjectionMatrix = FMatrix::PerspectiveFovLH(Fov, Aspect, Near, Far);
}

// Note: All other methods like Tick, Render, Duplicate, SetDecalTexture, etc., 
// are automatically inherited from UDecalComponent. We don't need to rewrite them here.