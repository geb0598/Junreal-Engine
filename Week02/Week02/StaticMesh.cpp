#include "pch.h"
#include "StaticMesh.h"
#include "MeshLoader.h"
#include "ResourceManager.h"
#include "ObjManager.h"

UStaticMesh::~UStaticMesh()
{
    ReleaseResources();
}

void UStaticMesh::Load(const FString& InFilePath, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
{
    assert(InDevice);
    
    VertexType = InVertexType;  // 버텍스 타입 저장

    StaticMeshAsset = FObjManager::LoadObjStaticMeshAsset(InFilePath);
	CreateVertexBuffer(StaticMeshAsset, InDevice, InVertexType);
	CreateIndexBuffer(StaticMeshAsset, InDevice);
	VertexCount = StaticMeshAsset->Vertices.size();
	IndexCount = StaticMeshAsset->Indices.size();

    /*MeshDataCPU = UMeshLoader::GetInstance().LoadMesh(InFilePath.c_str());
    CreateVertexBuffer(MeshDataCPU, InDevice, InVertexType);
    CreateIndexBuffer(MeshDataCPU, InDevice);
    VertexCount = MeshDataCPU->Vertices.size();
    IndexCount = MeshDataCPU->Indices.size();*/
}

void UStaticMesh::Load(FMeshData* InData, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
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

void UStaticMesh::CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
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
            vertex.Normal = (i < InMeshData->Normal.size()) ? InMeshData->Normal[i] : FVector(0,0,1);
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

void UStaticMesh::CreateVertexBuffer(FStaticMesh* InStaticMesh, ID3D11Device* InDevice, EVertexLayoutType InVertexType)
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

        std::vector<FVertexSimple> vertexArray;
        vertexArray.reserve(InStaticMesh->Vertices.size());

        for (size_t i = 0; i < InStaticMesh->Vertices.size(); ++i)
        {
            const FNormalVertex& src = InStaticMesh->Vertices[i];

            FVertexSimple vtx;
            vtx.Position = src.pos;
            vtx.Color = src.color;

            vertexArray.push_back(vtx);
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

        std::vector<FVertexDynamic> vertexArray;
        vertexArray.reserve(InStaticMesh->Vertices.size());

        for (size_t i = 0; i < InStaticMesh->Vertices.size(); ++i)
        {
            const FNormalVertex& src = InStaticMesh->Vertices[i];

            FVertexDynamic vtx;
            vtx.Position = src.pos;
            vtx.Color = src.color;
            vtx.UV = src.tex;
            vtx.Normal = FVector(src.normal.X, src.normal.Y, src.normal.Z);

            vertexArray.push_back(vtx);
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
        vertexArray.reserve(InStaticMesh->Vertices.size());

        for (size_t i = 0; i < InStaticMesh->Vertices.size(); ++i)
        {
            const FNormalVertex& src = InStaticMesh->Vertices[i];

            FBillboardVertexInfo_GPU info{};
            info.Position[0] = src.pos.X;
            info.Position[1] = src.pos.Y;
            info.Position[2] = src.pos.Z;

            info.CharSize[0] = src.tex.X;
            info.CharSize[1] = src.tex.Y;

            info.UVRect[0] = src.color.X;
            info.UVRect[1] = src.color.Y;
            info.UVRect[2] = src.color.Z;
            info.UVRect[3] = src.color.W;

            vertexArray.push_back(info);
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

    if (FAILED(hr))
    {
        UE_LOG("vertexBuffer 만들기 실패");
        assert(SUCCEEDED(hr));
    }
    //assert(SUCCEEDED(hr));
}

void UStaticMesh::CreateIndexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice)
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

void UStaticMesh::CreateIndexBuffer(FStaticMesh* InStaticMesh, ID3D11Device* InDevice)
{
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint32) * InStaticMesh->Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = InStaticMesh->Indices.data();

    HRESULT hr = InDevice->CreateBuffer(&ibd, &iinitData, &IndexBuffer);

    assert(SUCCEEDED(hr));
}

void UStaticMesh::ReleaseResources()
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
