#include "pch.h"
#include "Mesh.h"
#include "MeshLoader.h"
#include "ResourceManager.h"

void UMesh::Load(const FString& InFilePath, ID3D11Device* InDevice, EVertexType InVertexType)
{
    assert(InDevice);

	FMeshData* Data = UMeshLoader::GetInstance().LoadMesh(InFilePath.c_str());
    CreateVertexBuffer(Data, InDevice);
    CreateIndexBuffer(Data, InDevice);

    VertexCount = Data->Vertices.size();
    IndexCount = Data->Indices.size();
}

void UMesh::Load(FMeshData* InData, ID3D11Device* InDevice, EVertexType InVertexType)
{
    CreateVertexBuffer(InData, InDevice);
    CreateIndexBuffer(InData, InDevice);

    VertexCount = InData->Vertices.size();
    IndexCount = InData->Indices.size();
}

void UMesh::CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice)
{
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = static_cast<UINT>(sizeof(FVertexSimple) * InMeshData->Vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = InMeshData->Vertices.data();

    HRESULT hr = InDevice->CreateBuffer(&vbd, &vinitData, &VertexBuffer);

    assert(SUCCEEDED(hr));
}

void UMesh::CreateIndexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice)
{
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint32) * InMeshData->Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = InMeshData->Indices.data();

    HRESULT hr = InDevice->CreateBuffer(&ibd, &iinitData, &IndexBuffer);


    assert(SUCCEEDED(hr));
}
