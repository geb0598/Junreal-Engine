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
    //메테리얼 관리
    void SetMeshResource(const FString& FilePath);

    UMaterial* GetMaterial() const { return Material; }

    //// 정점 데이터 관리
    //void SetVertices(FVertexSimple* InVertices, int InCount)
    //{
    //    Vertices = InVertices;
    //    VertexCount = InCount;
    //}

    //FVertexSimple* GetVertices() const { return Vertices; }
    //int GetVertexCount() const { return VertexCount; }

protected:

    // 정점 데이터
    //FVertexSimple* Vertices;  // 정점 배열 주소
    //int VertexCount;          // 정점 개수

    UMesh* MeshResource = nullptr;
};