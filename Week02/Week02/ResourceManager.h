#pragma once
#include "ObjectFactory.h"
#include "Object.h"
#include "CharacterInfo.h"
#include "Shader.h"
#include "Mesh.h"
#include "Material.h"
#include "Texture.h"

class UStaticMesh;

class UResourceBase;
class UMesh;
class UMaterial;

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
    void Initialize(ID3D11Device* InDevice, ID3D11DeviceContext* InContext);

    ID3D11Device* GetDevice() { return Device; }

    //font 렌더링을 위함(dynamicVertexBuffer 만듦.)
    FResourceData* CreateOrGetResourceData(const FString& Name, uint32 Size, const TArray<uint32>& Indicies);
    //    FTextureData* GetOrCreateTexture

    UMaterial* GetOrCreateMaterial(const FString& Name,  EVertexLayoutType layoutType);

    void CreateDynamicVertexBuffer(FResourceData* data, uint32 Size, ID3D11Device* Device);
    void UpdateDynamicVertexBuffer(const FString& name, TArray<FBillboardCharInfo>& vertices);
    FTextureData* CreateOrGetTextureData(const FWideString& FilePath);

    // 전체 해제
    void Clear();

    void CreateAxisMesh(float Length, const FString& FilePath);
    void CreateGridMesh(int N, const FString& FilePath);
    void CreateDefaultShader();

    template<typename T>
    bool Add(const FString& InFilePath, UObject* InObject);
    template<typename T>
    T* Get(const FString& InFilePath);
    template<typename T, typename ... Args>
    T* Load(const FString& InFilePath, Args&& ...);
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

    TMap<FString, FResourceData*> ResourceMap;

    //Deprecated
    TMap<FWideString, FTextureData*> TextureMap;
    TMap<FString, UStaticMesh*> StaticMeshMap;

    //Resource Type의 개수만큼 Array 생성 및 저장
    TArray<TMap<FString, UResourceBase*>> Resources;

    FShader PrimitiveShader;
    TMap<FWideString,FShader*> ShaderList;

private:
    TMap<FString, UMaterial*> MaterialMap;
};

template<typename T>
bool UResourceManager::Add(const FString& InFilePath, UObject* InObject)
{
    uint8 typeIndex = static_cast<uint8>(GetResourceType<T>());
    auto iter = Resources[typeIndex].find(InFilePath);
    if (iter == Resources[typeIndex].end())
    {
        Resources[typeIndex][InFilePath] = static_cast<T*>(InObject);
        Resources[typeIndex][InFilePath]->SetFilePath(InFilePath);
        return true;
    }
    return false;
}

template<typename T>
T* UResourceManager::Get(const FString& InFilePath)
{
    uint8 typeIndex = static_cast<uint8>(GetResourceType<T>());
    auto iter = Resources[typeIndex].find(InFilePath);
    if (iter != Resources[typeIndex].end())
    {
        return static_cast<T*>(iter->second);
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
        T* Resource = NewObject<T>();
        Resource->Load(InFilePath, Device, std::forward<Args>(InArgs)...);
        Resource->SetFilePath(InFilePath);
        Resources[typeIndex][InFilePath] = Resource;
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
    if (T::StaticClass() == UMaterial::StaticClass())
        return ResourceType::Material;

    return ResourceType::None;
}