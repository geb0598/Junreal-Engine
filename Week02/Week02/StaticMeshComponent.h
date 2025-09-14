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
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }
    void SetStaticMesh(const FString& FilePath);
    void SetStaticMesh(UStaticMesh* InStaticMesh) { StaticMesh = InStaticMesh; }

    UMesh* GetMesh() { return MeshResource; }

  

    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
    
protected:
    //FString FilePath;
    //ResourceData* StaticMeshData = nullptr;

    UStaticMesh* StaticMesh = nullptr;

   
};

