#pragma once
#include <d3d11.h>
#include "UEContainer.h"
#include "Enums.h"
#include "MeshLoader.h"
#include "StaticMesh.h"
#include "Object.h"
#include "CharacterInfo.h"

class UResourceBase;
class UMesh;
class UShader;
class UTexture;
struct FShaderDesc
{
    std::wstring Filename;
    std::string EntryVS;
    std::string EntryPS;
    std::vector<D3D11_INPUT_ELEMENT_DESC> InputLayout;
};

class UResourceManager :public UObject
{
public:
    DECLARE_CLASS(UResourceManager, UObject)
    static UResourceManager& GetInstance();
    void Initialize(ID3D11Device* InDevice,ID3D11DeviceContext* InContext);

    UStaticMesh* GetOrCreateStaticMesh(const FString& FilePath);

    // 버텍스 버퍼 로드 (없으면 생성)
    FResourceData* GetOrCreateMeshBuffers(const FString& FilePath);

    //font 렌더링을 위함(dynamicVertexBuffer 만듦.)
    FResourceData* CreateOrGetResourceData(const FString& Name, uint32 Size , const TArray<uint32>& Indicies);
//    FTextureData* GetOrCreateTexture

    FShader* GetShader(const FWideString& Name);

    
    ID3D11Device* GetDevice() { return Device; }

    void CreateVertexBuffer(FResourceData* data, TArray<FVertexSimple>& vertices, ID3D11Device* device);
    void CreateIndexBuffer(FResourceData* data, const TArray<uint32>& indices, ID3D11Device* device);
    //void CreateIndexBuffer(FResourceData* data, const uint32 Size, ID3D11Device* device);
    void CreateDynamicVertexBuffer(FResourceData* data, uint32 Size, ID3D11Device* Device);
    void UpdateDynamicVertexBuffer(const FString& name, TArray<FBillboardCharInfo>& vertices);
    FTextureData* CreateOrGetTextureData(const FWideString& FilePath);
    void CreateShader(const FWideString& Name, const D3D11_INPUT_ELEMENT_DESC* Desc, uint32 Size);
    // 전체 해제
    void Clear();
    void CreateAxisMesh(float Length, const FString& FilePath);
    void CreateGridMesh(int N, const FString& FilePath);

    template<typename T>
    T* Get(const FString& InName);
    template<typename T, typename ... Args>
    T* Load(const FString& InName, Args&& ...);
    template<typename T>
    ResourceType GetResourceType();

public:
    UResourceManager() = default;
protected:
    ~UResourceManager() override;

    UResourceManager(const UResourceManager&) = delete;
    UResourceManager& operator=(const UResourceManager&) = delete;

    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* Context = nullptr;

    //Deprecated
    TMap<FString, FResourceData*> ResourceMap;
    TMap<FWideString, FTextureData*> TextureMap;
    TMap<FString, UStaticMesh*> StaticMeshMap;

    //Resource Type의 개수만큼 Array 생성 및 저장
    TArray<TMap<FString, UResourceBase*>> Resources;

    FShader PrimitiveShader;
    TMap<FWideString,FShader*> ShaderList;
};

template<typename T>
T* UResourceManager::Get(const FString& InFilePath)
{
    uint8 typeIndex = static_cast<uint8>(GetResourceType<T>());
    if (auto& iter = Resources[typeIndex].find(InFilePath))
    {
        return *iter;
    }

    return nullptr;
}

template<typename T, typename ...Args>
inline T* UResourceManager::Load(const FString& InFilePath, Args&&... InArgs)
{
    uint8 typeIndex = static_cast<uint8>(GetResourceType<T>());
    auto iter = Resources[typeIndex].find(InFilePath);
    if (iter != Resources[typeIndex].end())
    {
        return static_cast<T*>((*iter).second);
    }
    else
    {
        T* Resource = SpawnObject<T>();
        Resource->Load(InFilePath, Device, std::forward<Args>(InArgs)...);
        Resources[typeIndex].Add({ InFilePath, Resource });
        return Resource;
    }
}

template<typename T>
ResourceType UResourceManager::GetResourceType()
{
    if (T::StaticClass() == UMesh::StaticClass())
        return ResourceType::Mesh;
    if (T::StaticClass() == UShader::StaticClass())
        return ResourceType::Shader;
    if (T::StaticClass() == UTexture::StaticClass())
        return ResourceType::Texture;

    return ResourceType::None;
}