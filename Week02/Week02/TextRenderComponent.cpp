#include "pch.h"
#include "Enums.h"
#include "d3dtk/DDSTextureLoader.h"
#include "TextRenderComponent.h"
#include "ResourceManager.h"
#include "VertexData.h"
#include "CameraActor.h"
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
	

    //if(UResourceManager::GetInstance().Get<UMaterial>())
    MeshResource = UResourceManager::GetInstance().Get<UMesh>("TextBillboard");
    Material = NewObject<UMaterial>();

    
    
    //UResourceManager::GetInstance().Load<UMaterial>(AttachParent->GetWorldLocation(),);
    //ResourceData =UResourceManager::GetInstance().CreateOrGetResourceData(FString("TextBillboard"),sizeof(FBillboardCharInfo)*100,Indices);
    //TextureData = UResourceManager::GetInstance().CreateOrGetTextureData(FWideString(L"Font2.dds"));
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
    for (char c = 32; c <= 126;++c)
    {
        int key = c - 32;
        int col = key % COLROW;
        int row = key / COLROW;

        float pixelX = col * SUBTEX_WH;
        float pixelY = row * SUBTEX_WH;

        FBillboardVertexInfo CharInfo;
        CharInfo.UVRect.X = pixelX / TEXTURE_WH;
        CharInfo.UVRect.Y = pixelY / TEXTURE_WH;
        CharInfo.UVRect.Z = SUBTEX_WH / TEXTURE_WH;
        CharInfo.UVRect.W = CharInfo.UVRect.Z;

        CharInfoMap[c] = CharInfo;
    }
}

TArray<FBillboardVertexInfo_GPU> UTextRenderComponent::CreateVerticesForString(const FString& text, const FVector& StartPos) {
    TArray<FBillboardVertexInfo_GPU> vertices;
    FVector currentPos = StartPos;
    for (char c : text) {
        FBillboardVertexInfo_GPU info;
        info.Position[0] = currentPos.X;
        info.Position[1] = currentPos.Y;
        info.Position[2] = currentPos.Z;

        info.CharSize[0] = 1.f;
        info.CharSize[1] = 1.f;

        info.UVRect[0] = CharInfoMap[c].UVRect.X;
        info.UVRect[1] = CharInfoMap[c].UVRect.Y;
        info.UVRect[2] = CharInfoMap[c].UVRect.Z;
        info.UVRect[3] = CharInfoMap[c].UVRect.W;


        vertices.push_back(info); // 1번
        vertices.push_back(info); // 2번
        vertices.push_back(info); // 3번
        vertices.push_back(info); // 4번

        currentPos.X += info.CharSize[0]; // 커서 이동
    }
    return vertices;
}

void UTextRenderComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj)
{
    Renderer->OMSetBlendState(true);
    Material->Load("TextBillboard.dds", Renderer->GetRHIDevice()->GetDevice(), EVertexLayoutType::PositionBillBoard);//texture 불러오기 초기화는 ResourceManager Initialize() -> CreateTextBillboardTexture();
    ACameraActor* CameraActor =  GetOwner()->GetWorld()->GetCameraActor();
    FVector CamRight = CameraActor->GetActorRight();
    FVector CamUp = CameraActor->GetActorUp();
    Renderer->UpdateBillboardConstantBuffers(View, Proj, CamRight, CamUp);
    Renderer->PrepareShader(GetMaterial()->GetShader());
    TArray<FBillboardVertexInfo_GPU> vertices = CreateVerticesForString("HELLOWORLD", FVector(1.f, 1.f, 0.f));
    UResourceManager::GetInstance().UpdateDynamicVertexBuffer("TextBillboard", vertices);
    Renderer->DrawIndexedPrimitiveComponent(this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    Renderer->OMSetBlendState(false);
}