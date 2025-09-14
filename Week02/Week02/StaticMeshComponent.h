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
    UMesh* GetMesh() const { return MeshResource; }
    void SetMesh(const FString& FilePath);
    UShader* GetShader() const { return ShaderResource; }
    void SetShader(const FString& FilePath, EVertexLayoutType layoutType);
    UTexture* GetTexture() const { return TextureResource; }
    void SetTexture(const FString& FilePath);

protected:
    //FString FilePath;
    //ResourceData* StaticMeshData = nullptr;

    UMesh* MeshResource = nullptr;
    UShader* ShaderResource = nullptr;
    UTexture* TextureResource = nullptr;
};

