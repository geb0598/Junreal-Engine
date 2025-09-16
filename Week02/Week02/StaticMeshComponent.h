#pragma once
#include "MeshComponent.h"
#include "Enums.h"
#include "StaticMesh.h"

class StaticMesh;
class UMesh;
class UShader;
class UTexture;

class UStaticMeshComponent : public UMeshComponent
{
public:
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)
    UStaticMeshComponent();

protected:
    ~UStaticMeshComponent() override;

public:
    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
    
protected:
};

