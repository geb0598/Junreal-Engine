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
    SetMaterial("DecalShader.hlsl");

    // 3. Call our overridden update function to create the initial perspective matrix.
    UpdateDecalProjectionMatrix();
}

// Note: All other methods like Tick, Render, Duplicate, SetDecalTexture, etc., 
// are automatically inherited from UDecalComponent. We don't need to rewrite them here.