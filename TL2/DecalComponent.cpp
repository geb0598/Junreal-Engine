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
    // 기본 데칼 텍스처 로드

    SetMaterial("DecalShader.hlsl", EVertexLayoutType::PositionColorTexturNormal);
    if (Material)
    {
        Material->Load("Editor/Decal/SpotLight_64x.dds", UResourceManager::GetInstance().GetDevice());
    }
  
}

UDecalComponent::~UDecalComponent()
{
}

void UDecalComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj)
{
    if (!DecalBoxMesh || !Material)
        return;

    // 월드 행렬 생성
    FMatrix WorldMatrix = GetWorldMatrix();
    FMatrix InvWorldMatrix = WorldMatrix.InverseAffine();

    // ViewProj 행렬 및 역행렬 계산
    FMatrix ViewProj = View * Proj;
    FMatrix InvViewProj = ViewProj.InverseAffine();

    // 상수 버퍼 업데이트
    Renderer->UpdateConstantBuffer(WorldMatrix, View, Proj);
    Renderer->UpdateInvWorldBuffer(InvWorldMatrix, InvViewProj);

    // 데칼 셰이더 준비
    Renderer->PrepareShader(Material->GetShader());

    // 블렌드 스테이트 활성화 (반투명)
    Renderer->OMSetBlendState(true);

    // Depth SRV를 shader resource로 사용하기 위해 depth stencil view를 먼저 해제
    ID3D11RenderTargetView* currentRTV = nullptr;
    Renderer->GetRHIDevice()->GetDeviceContext()->OMGetRenderTargets(1, &currentRTV, nullptr);
    Renderer->GetRHIDevice()->GetDeviceContext()->OMSetRenderTargets(1, &currentRTV, nullptr);
    if (currentRTV) currentRTV->Release();

    // 데칼: 깊이 읽기만, 비교는 ALWAYS
    Renderer->OMSetDepthStencilState(EComparisonFunc::Always);

    // 앞면 컬링 (데칼 박스 내부만 그리기)
    Renderer->RSSetFrontCullState();

    // 데칼 박스 메쉬 렌더링
    UINT stride = sizeof(FVertexDynamic);
    UINT offset = 0;

    ID3D11Buffer* VertexBuffer = DecalBoxMesh->GetVertexBuffer();
    ID3D11Buffer* IndexBuffer = DecalBoxMesh->GetIndexBuffer();

    Renderer->GetRHIDevice()->GetDeviceContext()->IASetVertexBuffers(0, 1, &VertexBuffer, &stride, &offset);
    Renderer->GetRHIDevice()->GetDeviceContext()->IASetIndexBuffer(IndexBuffer, DXGI_FORMAT_R32_UINT, 0);
    Renderer->GetRHIDevice()->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    // 텍스처 바인딩
    if (Material->GetTexture())
    {
        ID3D11ShaderResourceView* TextureSRV = Material->GetTexture()->GetShaderResourceView();
        Renderer->GetRHIDevice()->GetDeviceContext()->PSSetShaderResources(0, 1, &TextureSRV);
    }

    // Sampler 바인딩
    Renderer->GetRHIDevice()->PSSetDefaultSampler(0);

    // Depth SRV 바인딩 (t1 슬롯)
    ID3D11ShaderResourceView* DepthSRV = static_cast<D3D11RHI*>(Renderer->GetRHIDevice())->GetDepthSRV();
    Renderer->GetRHIDevice()->GetDeviceContext()->PSSetShaderResources(1, 1, &DepthSRV);

    Renderer->GetRHIDevice()->GetDeviceContext()->DrawIndexed(DecalBoxMesh->GetIndexCount(), 0, 0);

    // SRV 리셋
    ID3D11ShaderResourceView* nullSRV[2] = { nullptr, nullptr };
    Renderer->GetRHIDevice()->GetDeviceContext()->PSSetShaderResources(0, 2, nullSRV);

    // Depth Stencil View 복원
    Renderer->GetRHIDevice()->OMSetRenderTargets();

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
