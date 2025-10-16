﻿#pragma once

#include "ResourceBase.h"
#include "Enums.h"
#include <d3d11.h>

class UTextQuad : public UResourceBase
{
public:
    DECLARE_CLASS(UTextQuad, UResourceBase)

    UTextQuad() = default;
    virtual ~UTextQuad() override;

    void Load(const FString& InFilePath, ID3D11Device* InDevice);
    void Load(FMeshData* InData, ID3D11Device* InDevice);

    ID3D11Buffer* GetVertexBuffer() const { return VertexBuffer; }
    ID3D11Buffer* GetIndexBuffer() const { return IndexBuffer; }
    uint32 GetVertexCount() { return VertexCount; }
    uint32 GetIndexCount() { return IndexCount; }
    void SetIndexCount(uint32 Cnt) { IndexCount = Cnt; }

    //const FString& GetAssetPathFileName() const { return StaticMeshAsset ? StaticMeshAsset->PathFileName : FilePath; }
    //void SetStaticMeshAsset(FStaticMesh* InStaticMesh) { StaticMeshAsset = InStaticMesh; }
    //FStaticMesh* GetStaticMeshAsset() const { return StaticMeshAsset; }
    //
    //const TArray<FGroupInfo>& GetMeshGroupInfo() const { return StaticMeshAsset->GroupInfos; }
    //bool HasMaterial() const { return StaticMeshAsset->bHasMaterial; }

private:
    void CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice);
    void CreateIndexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice);
    void ReleaseResources();

    // GPU 리소스
    ID3D11Buffer* VertexBuffer = nullptr;
    ID3D11Buffer* IndexBuffer = nullptr;
    uint32 VertexCount = 0;     // 정점 개수
    uint32 IndexCount = 0;     // 버텍스 점의 개수 
    // EVertexLayoutType VertexType = EVertexLayoutType::PositionColor;  // 버텍스 타입

    // CPU 리소스
    FMeshData* StaticMeshAsset = nullptr;
};