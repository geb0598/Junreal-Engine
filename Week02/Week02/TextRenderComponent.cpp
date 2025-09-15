#include "pch.h"
#include "Enums.h"
#include "d3dtk/DDSTextureLoader.h"
#include "TextRenderComponent.h"
#include "ResourceManager.h"
#include "CharacterInfo.h"

UTextRenderComponent::UTextRenderComponent()
{
    TArray<uint32> Indices;
    for (uint32 i = 0;i < 100;i++)
    {
        Indices.push_back(i * 4 + 0);
        Indices.push_back(i * 4 + 1);
        Indices.push_back(i * 4 + 2);

        Indices.push_back(i * 4 + 2);
        Indices.push_back(i * 4 + 1);
        Indices.push_back(i * 4 + 3);
    }
	
    
    ResourceData =UResourceManager::GetInstance().CreateOrGetResourceData(FString("TextBillboard"),sizeof(FBillboardCharInfo)*100,Indices);
    TextureData = UResourceManager::GetInstance().CreateOrGetTextureData(FWideString(L"Font2.dds"));
    InitCharInfoMap();
}

UTextRenderComponent::~UTextRenderComponent()
{

}



void UTextRenderComponent::InitCharInfoMap()
{
    const float TEXTURE_WH = 512.f;
    const float SUBTEX_WH = 32.f;
    const int COLROW = 16;

    FTextureData* Data = new FTextureData();
    for (wchar_t c = 32; c <= 126;++c)
    {
        int key = c - 32;
        int col = key % COLROW;
        int row = key / COLROW;

        float pixelX = col * SUBTEX_WH;
        float pixelY = row * SUBTEX_WH;

        FCharacterInfo CharInfo;
        CharInfo.Info.X = col / TEXTURE_WH;
        CharInfo.Info.Y = row / TEXTURE_WH;
        CharInfo.Info.Z = SUBTEX_WH / TEXTURE_WH;
        CharInfo.Info.W = CharInfo.Info.Z;

        CharInfoMap[c] = CharInfo;
    }
}

TArray<FBillboardCharInfo> UTextRenderComponent::CreateVerticesForString(const FString& text, const FVector& StartPos) {
    TArray<FBillboardCharInfo> vertices;
    FVector currentPos = StartPos;
    for (char c : text) {
        FBillboardCharInfo info;
        info.WorldPosition = currentPos;
        info.CharSize = { 1.0f, 1.0f };
        info.UVRect = CharInfoMap[c]; // Step 1에서 만든 데이터 사용

        vertices.push_back(info); // 1번
        vertices.push_back(info); // 2번
        vertices.push_back(info); // 3번
        vertices.push_back(info); // 4번

        currentPos.X += info.CharSize.X; // 커서 이동
    }
    return vertices;
}

void UTextRenderComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj)
{
    //URenderer::UpdateBillboardConstantBuffers(const FMatrix& ViewMatrix, const FMatrix& ProjMatrix, const FVector& CameraRight, const FVector& CameraUp)
    AttachParent->GetOwner()->GetWorld()->GetC
    //Renderer->UpdateBillboardConstantBuffers(View,Proj,)
//    Renderer->PrepareShader(GetMaterial()->GetShader());
  //  Renderer->DrawIndexedPrimitiveComponent(GetMeshResource(), D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
