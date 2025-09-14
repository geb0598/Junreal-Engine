#include "ResourceManager.h"
#include "ObjectFactory.h"
#include <d3d11.h>
#include <d3dcompiler.h>
#define GRIDNUM 100
#define AXISLENGTH 100

UResourceManager::~UResourceManager()
{
    Clear();
}

UResourceManager& UResourceManager::GetInstance()
{
    static UResourceManager* Instance = nullptr;
    if (Instance == nullptr)
    {
        Instance = NewObject<UResourceManager>();
    }
    return *Instance;
}

void UResourceManager::Initialize(ID3D11Device* InDevice)
{
    Device = InDevice;
    Resources.SetNum(static_cast<uint8>(ResourceType::End));

    CreateGridMesh(GRIDNUM,"Grid");
    CreateAxisMesh(AXISLENGTH,"Axis");
}

FResourceData* UResourceManager::GetOrCreateMeshBuffers(const FString& FilePath)
{
    auto it = ResourceMap.find(FilePath);
    if (it != ResourceMap.end())
    {
        return it->second; 
    }

    FMeshData* MeshData = UMeshLoader::GetInstance().LoadMesh(FilePath.c_str());

    FResourceData* ResourceData = new FResourceData();

    CreateVertexBuffer(ResourceData, MeshData->Vertices, Device);
    CreateIndexBuffer(ResourceData, MeshData->Indices, Device);
 
    ResourceMap[FilePath] = ResourceData;
    return ResourceData;
}

FShader& UResourceManager::GetPrimitiveShader()
{
    return PrimitiveShader;
}


void UResourceManager::CreatePrimitiveShader()
{
    ID3DBlob* vertexshaderCSO;
    ID3DBlob* pixelshaderCSO;

    D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

    Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &PrimitiveShader.SimpleVertexShader);

    D3DCompileFromFile(L"ShaderW0.hlsl", nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

    Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PrimitiveShader.SimplePixelShader);

    D3D11_INPUT_ELEMENT_DESC layout[] =
    {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };

    Device->CreateInputLayout(layout, ARRAYSIZE(layout), vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &PrimitiveShader.SimpleInputLayout);

    vertexshaderCSO->Release();
    pixelshaderCSO->Release();
}


UStaticMesh* UResourceManager::GetOrCreateStaticMesh(const FString& FilePath)
{
    auto it = StaticMeshMap.find(FilePath);
    if (it != StaticMeshMap.end())
    {
        return it->second;
    }

    UStaticMesh* StaticMesh = NewObject<UStaticMesh>();
    StaticMesh->SetFilePath(FilePath);

    StaticMeshMap.Add(FilePath, StaticMesh);
    // StaticMeshMap[FilePath] = StaticMesh;
    return StaticMesh;
}

// 전체 해제
void UResourceManager::Clear()
{
    { //Deprecated Part
        for (auto& [Key, Data] : StaticMeshMap)
        {
            if (Data)
            {
                ObjectFactory::DeleteObject(Data);
            }
        }
        StaticMeshMap.clear();

        for (auto& [Key, Data] : ResourceMap)
        {
            if (Data)
            {
                if (Data->VertexBuffer)
                {
                    Data->VertexBuffer->Release();
                    Data->VertexBuffer = nullptr;
                }
                if (Data->IndexBuffer)
                {
                    Data->IndexBuffer->Release();
                    Data->IndexBuffer = nullptr;
                }
                delete Data;
            }
        }
        ResourceMap.clear();

        // 이제 리소스 매니저에서 세이더를 삭제합니다
        if (PrimitiveShader.SimpleInputLayout)
        {
            PrimitiveShader.SimpleInputLayout->Release();
            PrimitiveShader.SimpleInputLayout = nullptr;
        }
        if (PrimitiveShader.SimpleVertexShader)
        {
            PrimitiveShader.SimpleVertexShader->Release();
            PrimitiveShader.SimpleVertexShader = nullptr;
        }
        if (PrimitiveShader.SimplePixelShader)
        {
            PrimitiveShader.SimplePixelShader->Release();
            PrimitiveShader.SimplePixelShader = nullptr;
        }
    }

    for (auto& Array : Resources)
    {
        for (auto& Resource : Array)
        {
            if(Resource.second)
            {
                DeleteObject(Resource.second);
                Resource.second = nullptr;
            }
        }
        Array.Empty();
    }
    Resources.Empty();

    // Instance lifetime is managed by ObjectFactory
}

void UResourceManager::CreateAxisMesh(float Length, const FString& FilePath)
{
    // 이미 있으면 패스
    if (ResourceMap[FilePath])
    {
        return;
    }

    TArray<FVertexSimple> axisVertices;
    TArray<uint32> axisIndices;

    // X축 (빨강)
    axisVertices.push_back({ 0.0f, 0.0f, 0.0f, 1,0,0,1 });       // 원점
    axisVertices.push_back({ Length, 0.0f, 0.0f, 1,0,0,1 });     // +X
    axisIndices.push_back(0);
    axisIndices.push_back(1);

    // Y축 (초록)
    axisVertices.push_back({ 0.0f, 0.0f, 0.0f, 0,0,1,1 });       // 원점
    axisVertices.push_back({ 0.0f, Length, 0.0f, 0,0,1,1 });     // +Y
    axisIndices.push_back(2);
    axisIndices.push_back(3);

    // Z축 (파랑)
    axisVertices.push_back({ 0.0f, 0.0f, 0.0f, 0,1,0,1 });       // 원점
    axisVertices.push_back({ 0.0f, 0.0f, Length, 0,1,0,1 });     // +Z
    axisIndices.push_back(4);
    axisIndices.push_back(5);

    // 리소스 데이터 생성
    FResourceData* data = new FResourceData();
    data->VertexCount = axisVertices.size();
    data->IndexCount = axisIndices.size();
    data->Topology = EPrimitiveTopology::LineList;

    // 버퍼 생성
    CreateVertexBuffer(data, axisVertices, Device);
    CreateIndexBuffer(data, axisIndices, Device);

    // 리소스 맵에 등록
    ResourceMap[FilePath] = data;
}
void UResourceManager::CreateGridMesh(int N, const FString& FilePath)
{
    if (ResourceMap[FilePath])
    {
        return;
    }
    TArray<FVertexSimple> gridVertices;
    TArray<uint32> gridIndices;
    // Z축 방향 선
    for (int i = -N; i <= N; i++)
    {
        if (i == 0)
        {
            continue;
        }
        // 색 결정: 5의 배수면 흰색, 아니면 회색
        float r = 0.1f, g = 0.1f, b = 0.1f;
        if (i % 5 == 0) { r = g = b = 0.4f; }
        if (i % 10 == 0) { r = g = b = 1.0f; }

        // 정점 2개 추가 (Z축 방향 라인)
        gridVertices.push_back({ (float)i, 0.0f, (float)-N, r,g,b,1 });
        gridVertices.push_back({ (float)i, 0.0f, (float)N, r,g,b,1 });

        // 인덱스 추가
        uint32 base = static_cast<uint32>(gridVertices.size());
        gridIndices.push_back(base - 2);
        gridIndices.push_back(base - 1);
    }

    // X축 방향 선
    for (int j = -N; j <= N; j++)
    {
        if (j == 0)
        {
            continue;
        }
        // 색 결정: 5의 배수면 흰색, 아니면 회색
        float r = 0.1f, g = 0.1f, b = 0.1f;

        if (j % 5 == 0) { r = g = b = 0.4f; }
        if (j % 10 == 0) { r = g = b = 1.0f; }

        // 정점 2개 추가 (X축 방향 라인)
        gridVertices.push_back({ (float)-N, 0.0f, (float)j, r,g,b,1 });
        gridVertices.push_back({ (float)N, 0.0f, (float)j, r,g,b,1 });

        // 인덱스 추가
        uint32 base = static_cast<uint32>(gridVertices.size());
        gridIndices.push_back(base - 2);
        gridIndices.push_back(base - 1);
    }

    gridVertices.push_back({ 0.0f, 0.0f, (float)-N, 1.0f,1.0f,1.0f,1.0f });
    gridVertices.push_back({ 0.0f, 0.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f });
    uint32 base = static_cast<uint32>(gridVertices.size());
    gridIndices.push_back(base - 2);
    gridIndices.push_back(base - 1);

    gridVertices.push_back({ (float)-N, 0.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f });
    gridVertices.push_back({ 0.0f, 0.0f, 0.0f, 1.0f,1.0f,1.0f,1.0f });
    base = static_cast<uint32>(gridVertices.size());
    gridIndices.push_back(base - 2);
    gridIndices.push_back(base - 1);

    FResourceData* data = new FResourceData();
    data->VertexCount = gridVertices.size();
    data->IndexCount = gridIndices.size();
    data->Topology = EPrimitiveTopology::LineList;

    // 버퍼 생성
    CreateVertexBuffer(data, gridVertices,Device);
    CreateIndexBuffer(data, gridIndices, Device);

    ResourceMap[FilePath] = data;
}


void UResourceManager::CreateVertexBuffer(FResourceData* data, TArray<FVertexSimple>& vertices, ID3D11Device* device)
{
    if (vertices.empty()) return;

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DEFAULT;
    vbd.ByteWidth = static_cast<UINT>(sizeof(FVertexSimple) * vertices.size());
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = vertices.data();

    HRESULT hr = device->CreateBuffer(&vbd, &vinitData, &data->VertexBuffer);
    if (FAILED(hr))
    {
        delete data;
        return;
    }

    data->VertexCount = static_cast<uint32>(vertices.size());
    data->ByteWidth = vbd.ByteWidth;
}

void UResourceManager::CreateIndexBuffer(FResourceData* data, const TArray<uint32>& indices, ID3D11Device* device)
{
    if (indices.empty()) return;

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint32) * indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = indices.data();

    HRESULT hr = device->CreateBuffer(&ibd, &iinitData, &data->IndexBuffer);
    if (FAILED(hr))
    {
        return;
    }

    data->IndexCount = static_cast<uint32>(indices.size());
}
