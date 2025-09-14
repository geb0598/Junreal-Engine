#pragma once
#include "ResourceBase.h"
#include "Enums.h"
#include <d3d11.h>

class UMesh : public UResourceBase
{
public:
    DECLARE_CLASS(UMesh, UResourceBase)

    void Load(const FString& InFilePath, ID3D11Device* InDevice);
    void Load(FMeshData* InData, ID3D11Device* InDevice);

    ID3D11Buffer* GetVertexBuffer() const { return VertexBuffer; }
    ID3D11Buffer* GetIndexBuffer() const { return IndexBuffer; }
    uint32 GetVertexCount() { return VertexCount; }
    uint32 GetIndexCount() { return IndexCount; }

private:
    void CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice);
    void CreateIndexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice);

    ID3D11Buffer* VertexBuffer = nullptr;
    ID3D11Buffer* IndexBuffer = nullptr;
    uint32 VertexCount = 0;     // 정점 개수
    uint32 IndexCount = 0;     // 버텍스 점의 개수 

    //Vertex Type 지정 필요할듯???
    //ex)UVVertex, ColorVertex, ...
};