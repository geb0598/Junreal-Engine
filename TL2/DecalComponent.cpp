// DecalComponent.cpp
#include "pch.h"
#include "DecalComponent.h"
#include "Renderer.h"
#include "ResourceManager.h"
#include "StaticMeshComponent.h"

IMPLEMENT_CLASS(UDecalComponent)

UDecalComponent::UDecalComponent()
{
    // 기본 큐브 메쉬 로드 (데칼 볼륨으로 사용)
    DecalBoxMesh = UResourceManager::GetInstance().Load<UStaticMesh>("Data/Cube.obj");
    // 기본 데칼 텍스처 로드

    SetMaterial("DecalShader.hlsl");
    if (Material)
    {
        Material->Load("Editor/Decal/SpotLight_64x.dds", UResourceManager::GetInstance().GetDevice());
    }
  
    UpdateDecalProjectionMatrix();
}

UDecalComponent::~UDecalComponent()
{
}

void UDecalComponent::SetDecalSize(const FVector& InSize)
{
    DecalSize = InSize;
    UpdateDecalProjectionMatrix();
}

void UDecalComponent::UpdateDecalProjectionMatrix()
{
    float Right = DecalSize.Y / 2.0f;
    float Left = -DecalSize.Y / 2.0f;
    float Top = DecalSize.Z / 2.0f;
    float Bottom = -DecalSize.Z / 2.0f;
    float Near = 0.0f;
    float Far = DecalSize.X / 2.0f;

    FMatrix OrthoMatrix = FMatrix::OffCenterOrthoLH(Left, Right, Top, Bottom, Near, Far);

    // UV 타일링을 위한 스케일 행렬 생성
    FMatrix ScaleMatrix = FMatrix::Identity();
    ScaleMatrix.M[0][0] = UVTiling.X;  // X 스케일
    ScaleMatrix.M[1][1] = UVTiling.Y;  // Y 스케일
    ScaleMatrix.M[2][2] = 1.0f;        // Z 스케일

    DecalProjectionMatrix = OrthoMatrix * ScaleMatrix;
}

void UDecalComponent::Render(URenderer* Renderer, UPrimitiveComponent* Component, const FMatrix& View, const FMatrix& Proj,FViewport* Viewport)
{
    if (!DecalBoxMesh || !Material)
        return;

    //일단 프로젝션 데칼 테스트용으로 StaticMesh만 처리
    UStaticMeshComponent* StaticMeshComponent = Cast<UStaticMeshComponent>(Component);
    if (!StaticMeshComponent)
    {
        return;
    }
    UStaticMesh* StaticMesh = StaticMeshComponent->GetStaticMesh();
    
    // 월드/역월드
    // DecalSize를 스케일로 적용, 데칼 world inverse를 구하기 위함
    FMatrix WorldMatrix = GetWorldMatrix();
    FMatrix InvWorldMatrix = WorldMatrix.InverseAffine(); // OK(Affine)

    //데칼 world inverse를 구했으므로 Componenent의 worldMatrix를 구해줌
    WorldMatrix = Component->GetWorldMatrix();

    // ViewProj 및 역행렬 (투영 포함 → 일반 Inverse 필요)
    FMatrix ViewProj = View * Proj;                   // row-major 기준

    // 상수 버퍼 업데이트
    //WorldMatrix = 데칼을 투영할 Component의 WorldMatrix
    Renderer->UpdateConstantBuffer(WorldMatrix, View, Proj);
    //InvWorldMatrix = 데칼의 WorldMatrixInverse
    Renderer->UpdateInvWorldBuffer(InvWorldMatrix, DecalProjectionMatrix);

    // 셰이더/블렌드 셋업
    Renderer->PrepareShader(Material->GetShader());
    Renderer->OMSetBlendState(true);                  // (SrcAlpha, InvSrcAlpha)인지 내부 확인

    // =========================
    // RTV 유지 + DSV 언바인드
    // =========================
    // FIX: 현재 RTV를 조회해서 DSV만 떼고 다시 바인딩
    ID3D11RenderTargetView* currentRTV = nullptr;
    ID3D11DeviceContext* ctx = Renderer->GetRHIDevice()->GetDeviceContext();

    ctx->OMGetRenderTargets(1, &currentRTV, nullptr);            // 현재 RTV 핸들 얻고
    ctx->OMSetRenderTargets(1, &currentRTV, nullptr);            // RTV 유지 + DSV 해제
    if (currentRTV) currentRTV->Release();                       // 로컬 ref release

    // 데칼은 깊이 "읽기"만 (LessEqual + DepthWrite Off)
    Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqualReadOnly);

    // 컬링 끄기(양면)
    Renderer->RSSetDefaultState();

    // 입력 어셈블러
    UINT stride = sizeof(FVertexDynamic);
    UINT offset = 0;
    ID3D11Buffer* vb = StaticMesh->GetVertexBuffer();
    ID3D11Buffer* ib = StaticMesh->GetIndexBuffer();;
    ctx->IASetVertexBuffers(0, 1, &vb, &stride, &offset);
    ctx->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);
    ctx->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    // 텍스처 & 샘플러
    if (Material->GetTexture())
    {
        ID3D11ShaderResourceView* texSRV = Material->GetTexture()->GetShaderResourceView();
        ctx->PSSetShaderResources(0, 1, &texSRV);
    }
    Renderer->GetRHIDevice()->PSSetDefaultSampler(0);

    // 서브매시 처리도 해줘야함.
    ctx->DrawIndexed(StaticMesh->GetIndexCount() , 0, 0);

    ID3D11ShaderResourceView* nullSRV = nullptr;
    ctx->PSSetShaderResources(0, 1, &nullSRV);

    // 원래 DSV/RTV 복원 (렌더러가 백버퍼/DSV 재바인딩)
    Renderer->GetRHIDevice()->OMSetRenderTargets();

    // 상태 복원
    Renderer->OMSetBlendState(false);
    Renderer->RSSetDefaultState();
    Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual); // 기본 상태로 복원
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
    UDecalComponent* DuplicatedComponent = Cast<UDecalComponent>(NewObject(GetClass()));
    if (DuplicatedComponent)
    {
        DuplicatedComponent->DecalSize = DecalSize;
        DuplicatedComponent->UVTiling = UVTiling;
        DuplicatedComponent->BlendMode = BlendMode;
    }
    return DuplicatedComponent;
}

void UDecalComponent::DuplicateSubObjects()
{
    UPrimitiveComponent::DuplicateSubObjects();
    // DecalBoxMesh는 공유 리소스이므로 복사하지 않음
}
