#include "pch.h"
#include "TextRenderComponent.h"
#include "Shader.h"
#include "Mesh.h"


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

void URenderer::PrepareShader(UShader* InShader)
{
    RHIDevice->GetDeviceContext()->VSSetShader(InShader->GetVertexShader(), nullptr, 0);
    RHIDevice->GetDeviceContext()->PSSetShader(InShader->GetPixelShader(), nullptr, 0);
    RHIDevice->GetDeviceContext()->IASetInputLayout(InShader->GetInputLayout());
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

void URenderer::RSSetState(EViewModeIndex ViewModeIndex)
{
    RHIDevice->RSSetState(ViewModeIndex);
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

void URenderer::DrawIndexedPrimitiveComponent(UMesh* InMesh, D3D11_PRIMITIVE_TOPOLOGY InTopology)
{
    if (!InMesh || !InMesh->GetVertexBuffer() || !InMesh->GetIndexBuffer()) return;

    UINT stride = sizeof(FVertexSimple);
    UINT offset = 0;

    ID3D11Buffer* VertexBuffer = InMesh->GetVertexBuffer();
    ID3D11Buffer* IndexBuffer = InMesh->GetIndexBuffer();
    uint32 VertexCount = InMesh->GetVertexCount();
    uint32 IndexCount = InMesh->GetIndexCount();

    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &VertexBuffer, &stride, &offset
    );

    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        IndexBuffer, DXGI_FORMAT_R32_UINT, 0
    );

    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(InTopology);
    RHIDevice->GetDeviceContext()->DrawIndexed(IndexCount, 0, 0);
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

    RHIDevice->GetDeviceContext()->PSGetSamplers(0, 1, &TextRenderComp->GetTextureData()->SamplerState);//문제 있는 코드 
    RHIDevice->GetDeviceContext()->PSSetShaderResources(0,1,&TextRenderComp->GetTextureData()->TextureSRV);
    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(Data->Topol);
    RHIDevice->GetDeviceContext()->DrawIndexed(Data->IndexCount, 0, 0);
}

void URenderer::EndFrame()
{
    RHIDevice->Present();
}


