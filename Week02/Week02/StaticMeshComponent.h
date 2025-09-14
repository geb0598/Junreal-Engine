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
    void SetMesh(const FString& FilePath);
    UShader* GetShader() { return ShaderResource; }
    void SetShader(const FString& FilePath, EVertexLayoutType layoutType);
    UTexture* GetTexture() { return TextureResource; }
    void SetTexture(const FString& FilePath);

    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
    
protected:
    //FString FilePath;
    //ResourceData* StaticMeshData = nullptr;

    UStaticMesh* StaticMesh = nullptr;

    UMesh* MeshResource = nullptr;
    UShader* ShaderResource = nullptr;
    UTexture* TextureResource = nullptr;
};

