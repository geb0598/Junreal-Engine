#pragma once

#include "pch.h"
#include "TextQuad.h"
#include "MeshLoader.h"
#include "ResourceManager.h"
#include "ObjManager.h"

UTextQuad::~UTextQuad()
{
    ReleaseResources();
}

void UTextQuad::Load(const FString& InFilePath, ID3D11Device* InDevice)
{
    assert(InDevice);

    if (VertexBuffer)
    {
        VertexBuffer->Release();
    }
    if (IndexBuffer)
    {
        IndexBuffer->Release();
    }

    CreateVertexBuffer(StaticMeshAsset, InDevice);
    CreateIndexBuffer(StaticMeshAsset, InDevice);
    VertexCount = StaticMeshAsset->Vertices.size();
    IndexCount = StaticMeshAsset->Indices.size();

    /*MeshDataCPU = UMeshLoader::GetInstance().LoadMesh(InFilePath.c_str());
    CreateVertexBuffer(MeshDataCPU, InDevice, InVertexType);
    CreateIndexBuffer(MeshDataCPU, InDevice);
    VertexCount = MeshDataCPU->Vertices.size();
    IndexCount = MeshDataCPU->Indices.size();*/
}

void UTextQuad::Load(FMeshData* InData, ID3D11Device* InDevice)
{
    if (VertexBuffer)
    {
        VertexBuffer->Release();
    }
    if (IndexBuffer)
    {
        IndexBuffer->Release();
    }

    CreateVertexBuffer(InData, InDevice);
    CreateIndexBuffer(InData, InDevice);

    VertexCount = InData->Vertices.size();
    IndexCount = InData->Indices.size();
}

void UTextQuad::CreateVertexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice)
{

    HRESULT hr = D3D11RHI::CreateVertexBuffer<FBillboardVertexInfo_GPU>(InDevice, *InMeshData, &VertexBuffer);
    assert(SUCCEEDED(hr));
}


void UTextQuad::CreateIndexBuffer(FMeshData* InMeshData, ID3D11Device* InDevice)
{
    HRESULT hr = D3D11RHI::CreateIndexBuffer(InDevice, InMeshData, &IndexBuffer);

    assert(SUCCEEDED(hr));
}


void UTextQuad::ReleaseResources()
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
