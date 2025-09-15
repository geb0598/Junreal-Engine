#include "pch.h"
#include "Mesh.h"
#include "MeshLoader.h"
#include "ResourceManager.h"

UMesh::~UMesh()
{
    ReleaseResources();
}

void UMesh::Load(const FString& InFilePath, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
{
    assert(InDevice);
    
    VertexType = InVertexType;  // 버텍스 타입 저장

    MeshDataCPU = UMeshLoader::GetInstance().LoadMesh(InFilePath.c_str());
//	FMeshData* Data = UMeshLoader::GetInstance().LoadMesh(InFilePath.c_str());
    CreateVertexBuffer(MeshDataCPU, InDevice, InVertexType);
    CreateIndexBuffer(MeshDataCPU, InDevice);

    VertexCount = MeshDataCPU->Vertices.size();
    IndexCount = MeshDataCPU->Indices.size();
}

void UMesh::Load(FMeshData* InData, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
{
    VertexType = InVertexType;  // 버텍스 타입 저장
    
    CreateVertexBuffer(InData, InDevice, InVertexType);
    CreateIndexBuffer(InData, InDevice);

    VertexCount = InData->Vertices.size();
    IndexCount = InData->Indices.size();
}

void UMesh::CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
{
    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    
    D3D11_SUBRESOURCE_DATA vinitData = {};
    HRESULT hr;
    
    switch (InVertexType)
    {
    case EVertexLayoutType::PositionColor:
    {
        // FMeshData의 Vertices + Color를 FVertexSimple로 조합
        std::vector<FVertexSimple> vertexArray;
        vertexArray.reserve(InMeshData->Vertices.size());
        
        for (size_t i = 0; i < InMeshData->Vertices.size(); ++i)
        {
            FVertexSimple vertex;
            vertex.Position = InMeshData->Vertices[i];
            vertex.Color = (i < InMeshData->Color.size()) ? InMeshData->Color[i] : FVector4(1,1,1,1);
            vertexArray.push_back(vertex);
        }
        
        vbd.ByteWidth = static_cast<UINT>(sizeof(FVertexSimple) * vertexArray.size());
        vinitData.pSysMem = vertexArray.data();
        
        hr = InDevice->CreateBuffer(&vbd, &vinitData, &VertexBuffer);
        break;
    }
    case EVertexLayoutType::PositionColorTexturNormal:
    {
        // FMeshData의 모든 데이터를 FVertexDynamic으로 조합
        std::vector<FVertexDynamic> vertexArray;
        vertexArray.reserve(InMeshData->Vertices.size());
        
        for (size_t i = 0; i < InMeshData->Vertices.size(); ++i)
        {
            FVertexDynamic vertex;
            vertex.Position = InMeshData->Vertices[i];
            vertex.Color = (i < InMeshData->Color.size()) ? InMeshData->Color[i] : FVector4(1,1,1,1);
            vertex.UV = (i < InMeshData->UV.size()) ? InMeshData->UV[i] : FVector2D(0,0);
            vertex.Normal = (i < InMeshData->Normal.size()) ? InMeshData->Normal[i] : FVector4(0,0,1,0);
            vertexArray.push_back(vertex);
        }
        
        vbd.ByteWidth = static_cast<UINT>(sizeof(FVertexDynamic) * vertexArray.size());
        vinitData.pSysMem = vertexArray.data();
        
        hr = InDevice->CreateBuffer(&vbd, &vinitData, &VertexBuffer);
        break;
    }
    default:
        assert(false && "Unknown VertexType");
        return;
    }
    
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

void UMesh::ReleaseResources()
{
    if (VertexBuffer)
    {
        VertexBuffer->Release();
    }
    if (IndexBuffer)
    {
        IndexBuffer->Release();
    }
}
