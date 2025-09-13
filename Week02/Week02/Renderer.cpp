#include "Renderer.h"
#include "TextRenderComponent.h"
#include "D3D11RHI.h"


URenderer::URenderer(URHIDevice* InDevice) : RHIDevice(InDevice)
{

}
URenderer::~URenderer()
{

}
void URenderer::BeginFrame()
{
    // 백버퍼/깊이버퍼를 클리어
    RHIDevice->ClearBackBuffer();  // 배경색
    RHIDevice->ClearDepthBuffer(1.0f, 0);                 // 깊이값 초기화
    RHIDevice->CreateBlendState();

    RHIDevice->IASetPrimitiveTopology();

    // RS
    RHIDevice->RSSetViewport();
    RHIDevice->RSSetState();

    //OM
    //RHIDevice->OMSetBlendState();
    RHIDevice->OMSetRenderTargets();
}

void URenderer::PrepareShader(FShader& InShader)
{
    RHIDevice->GetDeviceContext()->VSSetShader(InShader.SimpleVertexShader, nullptr, 0);
    RHIDevice->GetDeviceContext()->PSSetShader(InShader.SimplePixelShader, nullptr, 0);
    RHIDevice->GetDeviceContext()->IASetInputLayout(InShader.SimpleInputLayout);
}

void URenderer::OMSetBlendState(bool bIsChecked)
{
    if (bIsChecked == true)
    {
        RHIDevice->OMSetBlendState(true);
    }
    else
    {
        RHIDevice->OMSetBlendState(false);
    }
}

void URenderer::UpdateConstantBuffer(const FMatrix& ModelMatrix, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix)
{
    RHIDevice->UpdateConstantBuffers(ModelMatrix, ViewMatrix, ProjMatrix);
}

void URenderer::UpdateHighLightConstantBuffer(const float InPicked, const FVector& InColor, const uint32 X, const uint32 Y, const uint32 Z, const uint32 Gizmo)
{
    RHIDevice->UpdateHighLightConstantBuffers(InPicked, InColor, X, Y, Z, Gizmo);
}

void URenderer::UpdateBillboardConstantBuffers(const FMatrix& ViewMatrix, const FMatrix& ProjMatrix, const FVector& CameraRight, const FVector& CameraUp)
{
    RHIDevice->UpdateBillboardConstantBuffers(ViewMatrix, ProjMatrix, CameraRight, CameraUp);
}

void URenderer::DrawIndexedPrimitiveComponent(UStaticMeshComponent* MeshComp)
{
    if (!MeshComp || !MeshComp->GetStaticMesh() || !MeshComp->GetStaticMesh()->GetResourceData()) return;

    FResourceData* Data = MeshComp->GetStaticMesh()->GetResourceData();
    //TODO : Set Shader
    UINT stride = sizeof(FVertexSimple);
    UINT offset = 0;

    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &Data->VertexBuffer, &stride, &offset
    );

    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        Data->IndexBuffer, DXGI_FORMAT_R32_UINT, 0
    );

    D3D11_PRIMITIVE_TOPOLOGY dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    switch (Data->Topology)
    {
    case EPrimitiveTopology::PointList:     dxTopology = D3D11_PRIMITIVE_TOPOLOGY_POINTLIST; break;
    case EPrimitiveTopology::LineList:      dxTopology = D3D11_PRIMITIVE_TOPOLOGY_LINELIST; break;
    case EPrimitiveTopology::LineStrip:     dxTopology = D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP; break;
    case EPrimitiveTopology::TriangleList:  dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST; break;
    case EPrimitiveTopology::TriangleStrip: dxTopology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP; break;
    }

    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(dxTopology);
    RHIDevice->GetDeviceContext()->DrawIndexed(Data->IndexCount, 0, 0);
}

void URenderer::DrawIndexedPrimitiveComponent(UTextRenderComponent* TextRenderComp)
{
    FResourceData* Data = TextRenderComp->GetResourceData();
    //TODO : Set Shader
    TArray<FBillboardCharInfo> Infos = TextRenderComp->CreateVerticesForString(FString("Hello"), FVector(0, 0, 0));
    UResourceManager::GetInstance().UpdateDynamicVertexBuffer("TextBillboard", Infos);
    UINT Stride = sizeof(FBillboardCharInfo);
    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &Data->VertexBuffer,&Stride, &Data->Offset
    );
    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        Data->IndexBuffer, DXGI_FORMAT_R32_UINT, 0
    );

    RHIDevice->GetDeviceContext()->PSGetSamplers(0, 1, &TextRenderComp->GetTextureData()->SamplerState);
    RHIDevice->GetDeviceContext()->PSSetShaderResources(0,1,&TextRenderComp->GetTextureData()->TextureSRV);
    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(Data->Topol);
    RHIDevice->GetDeviceContext()->DrawIndexed(Data->IndexCount, 0, 0);
}

void URenderer::EndFrame()
{
    RHIDevice->Present();
}


