#pragma once
#include "PrimitiveComponent.h"

class UMeshComponent : public UPrimitiveComponent
{
public:
    DECLARE_CLASS(UMeshComponent, UPrimitiveComponent)
    UMeshComponent();

protected:
    ~UMeshComponent() override;

public:
    virtual void SetMeshResource(const FString& FilePath);
    virtual UMesh* GetMeshResource() const { return MeshResource; }


protected:

    // 정점 데이터
    //FVertexSimple* Vertices;  // 정점 배열 주소
    //int VertexCount;          // 정점 개수

    UMesh* MeshResource = nullptr;
};