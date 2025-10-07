// DecalComponent.cpp
#include "pch.h"
#include "DecalComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"

IMPLEMENT_CLASS(UDecalComponent)

UDecalComponent::UDecalComponent()
{
    // 기본 큐브 메쉬 로드 (데칼 볼륨으로 사용)
    DecalBoxMesh = UResourceManager::GetInstance().Load<UStaticMesh>("cube-tex.obj");
    SetMaterial("DecalShader.hlsl", EVertexLayoutType::PositionColorTexturNormal);

    // 기본 데칼 텍스처 로드
    if (Material)
    {
        Material->Load("Editor/Icon/PointLight_64x.dds", UResourceManager::GetInstance().GetDevice());
    }
}

UDecalComponent::~UDecalComponent()
{
}

void UDecalComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj)
{
    if (!DecalBoxMesh || !Material)
        return;

    // 데칼 크기를 적용한 월드 행렬 생성
   // FMatrix ScaleMatrix = FMatrix::FromTRS(DecalSize);
    //FMatrix WorldMatrix = ScaleMatrix * GetWorldMatrix();
    FMatrix WorldMatrix = GetWorldMatrix();

    // 상수 버퍼 업데이트
    Renderer->UpdateConstantBuffer(WorldMatrix, View, Proj);

    // 데칼 셰이더 준비
    Renderer->PrepareShader(Material->GetShader());

    // 블렌드 스테이트 활성화 (반투명)
    Renderer->OMSetBlendState(true);

    // Depth 테스트는 하되 쓰기는 하지 않음
    Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqualReadOnly);

    // 앞면 컬링 (데칼 박스 내부만 그리기)
    Renderer->RSSetState(EViewModeIndex::VMI_Lit);

    // 데칼 박스 메쉬 렌더링
    UINT stride = sizeof(FVertexDynamic);
    UINT offset = 0;

    ID3D11Buffer* VertexBuffer = DecalBoxMesh->GetVertexBuffer();
    ID3D11Buffer* IndexBuffer = DecalBoxMesh->GetIndexBuffer();

    Renderer->GetRHIDevice()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
    Renderer->GetRHIDevice()->GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Renderer->GetRHIDevice()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    FTextureData* TextureData = nullptr;

    bool bHasTexture = !(Material->GetMaterialInfo().DiffuseTextureFileName == FName::None());
  /*  if (bHasTexture)
    {
        TextureData = UResourceManager::GetInstance().CreateOrGetTextureData("cube_texture.dds");
        Renderer->GetRHIDevice()->GetDeviceContext()->PSSetShaderResources(0, 1, &(TextureData->TextureSRV));
    }*/
    // 텍스처 바인딩Renderer->GetRHIDevice()
    if (Material->GetTexture())
    {
        ID3D11ShaderResourceView* TextureSRV = Material->GetTexture()->GetShaderResourceView();
        Renderer->GetRHIDevice()->GetDeviceContext()->PSSetShaderResources(0, 1, &TextureSRV);
    }

    // Depth SRV 바인딩 (t1 슬롯)
    ID3D11ShaderResourceView* DepthSRV = static_cast<D3D11RHI*>(Renderer->GetRHIDevice())->GetDepthSRV();
    Renderer->GetRHIDevice()->GetDeviceContext()->PSSetShaderResources(1, 1, &DepthSRV);

    Renderer->GetRHIDevice()->GetDeviceContext()->DrawIndexed(DecalBoxMesh->GetIndexCount(), 0, 0);

    // 상태 복원
    Renderer->OMSetBlendState(false);
    Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
}

void UDecalComponent::SetDecalTexture(const FString& TexturePath)
{
    if (!Material)
        return;

    // TextRenderComponent와 동일한 방식으로 텍스처 로드
    Material->Load(TexturePath, UResourceManager::GetInstance().GetDevice());
}

UObject* UDecalComponent::Duplicate()
{
    UDecalComponent* NewComp = static_cast<UDecalComponent*>(UPrimitiveComponent::Duplicate());
    if (NewComp)
    {
        NewComp->DecalSize = DecalSize;
        NewComp->BlendMode = BlendMode;
    }
    return NewComp;
}

void UDecalComponent::DuplicateSubObjects()
{
    UPrimitiveComponent::DuplicateSubObjects();
    // DecalBoxMesh는 공유 리소스이므로 복사하지 않음
}
