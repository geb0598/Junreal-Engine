#include "pch.h"
#include "MeshLoader.h"
#include "ObjectFactory.h"
#include "Mesh.h"
#include "Shader.h"
#include "Texture.h"
#include "d3dtk/DDSTextureLoader.h"
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

void UResourceManager::Initialize(ID3D11Device* InDevice, ID3D11DeviceContext* InContext)
{
    Device = InDevice;
    Resources.SetNum(static_cast<uint8>(ResourceType::End));

    Context = InContext;
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


FResourceData* UResourceManager::CreateOrGetResourceData(const FString& Name, uint32 Size , const TArray<uint32>& Indicies)
{
    auto it = ResourceMap.find(Name);
    if(it!=ResourceMap.end())
    {
        return it->second;
    }

    FResourceData* ResourceData = new FResourceData();

    CreateDynamicVertexBuffer(ResourceData, Size, Device);
    CreateIndexBuffer(ResourceData, Indicies, Device);

    ResourceMap[Name] = ResourceData;
    return ResourceData;
}

FShader* UResourceManager::GetShader(const FWideString& Name)
{
    auto it = ShaderList.find(Name);
    if(it!=ShaderList.end())
        return ShaderList[Name];
    return nullptr;
}

void UResourceManager::CreateShader(const FWideString& Name,const D3D11_INPUT_ELEMENT_DESC* Desc, uint32 Size)
{
    auto it = ShaderList.find(Name);
    if (it != ShaderList.end()) return;
    ID3DBlob* vertexshaderCSO;
    ID3DBlob* pixelshaderCSO;

    ID3D11VertexShader* VS;
    ID3D11PixelShader* PS;
    ID3D11InputLayout* Layout;
    HRESULT hr = D3DCompileFromFile(Name.c_str(), nullptr, nullptr, "mainVS", "vs_5_0", 0, 0, &vertexshaderCSO, nullptr);

    hr = Device->CreateVertexShader(vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), nullptr, &VS);

    hr = D3DCompileFromFile(Name.c_str(), nullptr, nullptr, "mainPS", "ps_5_0", 0, 0, &pixelshaderCSO, nullptr);

    hr = Device->CreatePixelShader(pixelshaderCSO->GetBufferPointer(), pixelshaderCSO->GetBufferSize(), nullptr, &PS);

    hr = Device->CreateInputLayout(Desc, Size, vertexshaderCSO->GetBufferPointer(), vertexshaderCSO->GetBufferSize(), &Layout);
    FShader* Shader = new FShader( Layout,VS,PS );
    ShaderList[Name] = Shader;
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

    FMeshData* MeshData = new FMeshData();
    MeshData->Vertices = axisVertices;
    MeshData->Indices = axisIndices;

    UMesh* Mesh = NewObject<UMesh>();
    Mesh->Load(MeshData, Device);
    Add<UMesh>("Axis", Mesh);
    
    delete MeshData;
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

    FMeshData* MeshData = new FMeshData();
    MeshData->Vertices = gridVertices;
    MeshData->Indices = gridIndices;

    UMesh* Mesh = NewObject<UMesh>();
    Mesh->Load(MeshData, Device);
    Add<UMesh>("Grid", Mesh);

    delete MeshData;
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

void UResourceManager::CreateDynamicVertexBuffer(FResourceData* data, uint32 Size, ID3D11Device* Device)
{
    if (Size == 0) return;

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = D3D11_USAGE_DYNAMIC;
    vbd.ByteWidth = static_cast<UINT>(sizeof(FBillboardCharInfo) * Size);
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    D3D11_SUBRESOURCE_DATA vinitData = {};

    HRESULT hr = Device->CreateBuffer(&vbd, nullptr, &data->VertexBuffer);
    if (FAILED(hr))
    {
        delete data;
        return;
    }

    data->VertexCount = static_cast<uint32>(Size);
    data->ByteWidth = vbd.ByteWidth;
}


void UResourceManager::UpdateDynamicVertexBuffer(const FString& Name, TArray<FBillboardCharInfo>& vertices)
{
    if (ResourceMap.find(Name) == ResourceMap.end()) return;
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    Context->Map(ResourceMap[Name]->VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);//리소스 데이터의 버텍스 데이터를 mappedResource에 매핑
    memcpy(mappedResource.pData, vertices.data(), sizeof(FBillboardCharInfo) * vertices.size()); //vertices.size()만큼의 Character info를 vertices에서 pData로 복사해가라
    Context->Unmap(ResourceMap[Name]->VertexBuffer, 0);//언맵
}

FTextureData* UResourceManager::CreateOrGetTextureData(const FWideString& FilePath)
{
    auto it = TextureMap.find(FilePath);
    if (it!=TextureMap.end())
    {
        return it->second;
    }

    FTextureData* Data = new FTextureData();
    HRESULT hr = DirectX::CreateDDSTextureFromFile(Device, FilePath.c_str(), &Data->Texture, &Data->TextureSRV, 0, nullptr);
    if (FAILED(hr))
    {
        int a = 0;
        D3D11_SAMPLER_DESC SamplerDesc;
        ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
    }
    D3D11_SAMPLER_DESC SamplerDesc;
    ZeroMemory(&SamplerDesc, sizeof(SamplerDesc));
    SamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    SamplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SamplerDesc.MinLOD = 0;
    SamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

    hr = Device->CreateSamplerState(&SamplerDesc, &Data->SamplerState);
    if (FAILED(hr))
    {

    }

    D3D11_BLEND_DESC blendDesc;
    ZeroMemory(&blendDesc, sizeof(blendDesc));
    blendDesc.RenderTarget[0].BlendEnable = TRUE;
    blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    // 멤버 변수 m_pAlphaBlendState에 저장
    hr = Device->CreateBlendState(&blendDesc, &Data->BlendState);
    if (FAILED(hr))
    {

    }
    TextureMap[FilePath] = Data;
    return TextureMap[FilePath];
}
