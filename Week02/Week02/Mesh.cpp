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

    if (VertexBuffer)
    {
        VertexBuffer->Release();
    }
    if (IndexBuffer)
    {
        IndexBuffer->Release();
    }
    
    CreateVertexBuffer(InData, InDevice, InVertexType);
    CreateIndexBuffer(InData, InDevice);

    VertexCount = InData->Vertices.size();
    IndexCount = InData->Indices.size();
}

void UMesh::CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
{
    D3D11_BUFFER_DESC vbd = {};
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA vinitData = {};
    HRESULT hr;
    
    switch (InVertexType)
    {
    case EVertexLayoutType::PositionColor:
    {
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.CPUAccessFlags = 0;
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
        vbd.Usage = D3D11_USAGE_DEFAULT;
        vbd.CPUAccessFlags = 0;
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
    case EVertexLayoutType::PositionBillBoard:
    {
        vbd.Usage = D3D11_USAGE_DYNAMIC;
        vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        std::vector<FBillboardVertexInfo_GPU> vertexArray;
        vertexArray.reserve(InMeshData->Vertices.size());//billboard world position
        for (size_t i = 0;i < InMeshData->Vertices.size();++i)
        {
            FBillboardVertexInfo_GPU VertexInfo;
            VertexInfo.Position[0] = InMeshData->Vertices[i].X;
            VertexInfo.Position[1] = InMeshData->Vertices[i].Y;
            VertexInfo.Position[2] = InMeshData->Vertices[i].Z;

            VertexInfo.CharSize[0] = InMeshData->UV[i].X;
            VertexInfo.CharSize[1] = InMeshData->UV[i].Y;

            VertexInfo.UVRect[0] = InMeshData->Color[i].X;
            VertexInfo.UVRect[1] = InMeshData->Color[i].Y;
            VertexInfo.UVRect[2] = InMeshData->Color[i].Z;
            VertexInfo.UVRect[3] = InMeshData->Color[i].W;
            vertexArray.push_back(VertexInfo);
        }
        vbd.ByteWidth = static_cast<UINT>(sizeof(FBillboardVertexInfo_GPU) * vertexArray.size());
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
