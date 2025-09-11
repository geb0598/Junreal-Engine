#pragma once
#include <d3d11.h>
#include "UEContainer.h"
#include "Enums.h"
#include "MeshLoader.h"
#include "StaticMesh.h"
#include "Object.h"
class UResourceManager :public UObject

{
public:
    DECLARE_CLASS(UResourceManager, UObject)
    static UResourceManager& GetInstance();
    void Initialize(ID3D11Device* InDevice);

    UStaticMesh* GetOrCreateStaticMesh(const FString& FilePath);

    // 버텍스 버퍼 로드 (없으면 생성)
    FResourceData* GetOrCreateMeshBuffers(const FString& FilePath);

    FShader& GetPrimitiveShader();

    
    void CreateVertexBuffer(FResourceData* data, TArray<FVertexSimple>& vertices, ID3D11Device* device);
    void CreateIndexBuffer(FResourceData* data, const TArray<uint32>& indices, ID3D11Device* device);
    void CreatePrimitiveShader();
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

    TMap<FString, FResourceData*> ResourceMap;
    TMap<FString, UStaticMesh*> StaticMeshMap;

    FShader PrimitiveShader;
};
