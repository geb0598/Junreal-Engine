#pragma once
#include "MeshComponent.h"
#include "Enums.h"
#include "StaticMesh.h"

class UStaticMesh;
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

    void SetStaticMesh(const FString& PathFileName);
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }
    
protected:
    UStaticMesh* StaticMesh = nullptr;
};

