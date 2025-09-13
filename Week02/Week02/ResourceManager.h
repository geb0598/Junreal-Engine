#pragma once
#include <d3d11.h>
#include "UEContainer.h"
#include "Enums.h"
#include "MeshLoader.h"
#include "StaticMesh.h"
#include "Object.h"
#include "CharacterInfo.h"
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

    
    void CreateVertexBuffer(FResourceData* data,const TArray<FVertexSimple>& vertices, ID3D11Device* device);
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

public:
    UResourceManager() = default;
protected:
    ~UResourceManager() override;

    UResourceManager(const UResourceManager&) = delete;
    UResourceManager& operator=(const UResourceManager&) = delete;

    ID3D11Device* Device = nullptr;
    ID3D11DeviceContext* Context = nullptr;

    TMap<FString, FResourceData*> ResourceMap;
    TMap<FWideString, FTextureData*> TextureMap;
    TMap<FString, UStaticMesh*> StaticMeshMap;

    TMap<FWideString,FShader*> ShaderList;
};
