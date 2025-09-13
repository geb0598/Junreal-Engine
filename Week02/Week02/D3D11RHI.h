#pragma once
#include "RHIDevice.h"
#include "ResourceManager.h"

class D3D11RHI : public URHIDevice
{
public:
    D3D11RHI() {};
    ~D3D11RHI()
    {
        Release();
    }



public:
    void Initialize(HWND hWindow) override;

    void Release()override;


public:
    // clear
    void ClearBackBuffer() override;
    void ClearDepthBuffer(float Depth, UINT Stencil) override;
    void CreateBlendState() override;

    void UpdateConstantBuffers(const FMatrix& ModelMatrix, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix) override;
    void UpdateBillboardConstantBuffers(const FMatrix& ViewMatrix, const FMatrix& ProjMatrix, const FVector& CameraRight, const FVector& CameraUp) override;
    void UpdateHighLightConstantBuffers(const uint32 InPicked, const FVector& InColor, const uint32 X, const uint32 Y, const uint32 Z, const uint32 Gizmo) override;

    void IASetPrimitiveTopology() override;
    void RSSetViewport() override;
    void RSSetState() override;
    void OMSetRenderTargets() override;
    void OMSetBlendState(bool bIsBlendMode) override;
    void Present() override;



    void CreateShader(ID3D11InputLayout** OutSimpleInputLayout, ID3D11VertexShader** OutSimpleVertexShader, ID3D11PixelShader** OutSimplePixelShader) override;

    void OnResize(UINT NewWidth, UINT NewHeight);

    void CreateBackBufferAndDepthStencil(UINT width, UINT height);

    void SetViewport(UINT width, UINT height);

    void setviewort(UINT width, UINT height);

    void ResizeSwapChain(UINT width, UINT height);

public:
    // getter
    inline ID3D11Device* GetDevice()
    {
        return Device;
    }
    inline ID3D11DeviceContext* GetDeviceContext()
    {
        return DeviceContext;
    }

private:
    void CreateDeviceAndSwapChain(HWND hWindow)override; // 여기서 디바이스, 디바이스 컨택스트, 스왑체인, 뷰포트를 초기화한다
    void CreateFrameBuffer() override;
    void CreateRasterizerState() override;
    void CreateConstantBuffer() override;
    
    // release
    void ReleaseBlendState();
    void ReleaseRasterizerState(); // rs
    void ReleaseFrameBuffer(); // fb, rtv
    void ReleaseDeviceAndSwapChain();
 
    
private:
    //24
    D3D11_VIEWPORT ViewportInfo{};

    //8
    ID3D11Device* Device{};//
    ID3D11DeviceContext* DeviceContext{};//
    IDXGISwapChain* SwapChain{};//

    ID3D11RenderTargetView* RenderTargetView{};//
    ID3D11DepthStencilView* DepthStencilView{};//

    ID3D11RasterizerState* RasterizerState{};//
    ID3D11DepthStencilState* DepthStencilState{};
    ID3D11BlendState* BlendState{};

    ID3D11Texture2D* FrameBuffer{};//
    ID3D11Buffer* ConstantBuffer{};

    // 버퍼 핸들
    ID3D11Buffer* ModelCB{};
    ID3D11Buffer* ViewProjCB{};
    ID3D11Buffer* HighLightCB{};
    ID3D11Buffer* BillboardCB{};
};

