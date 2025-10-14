#include "pch.h"
#include "UI/StatsOverlayD2D.h"
#include "D3D11RHI.h"

void D3D11RHI::Initialize(HWND hWindow)
{
    // 이곳에서 Device, DeviceContext, viewport, swapchain를 초기화한다
    CreateDeviceAndSwapChain(hWindow);
    CreateFrameBuffer();
    CreateRasterizerState();
    CreateBlendState();
    CreateConstantBuffer();
	CreateDepthStencilState();
	CreateSamplerState();
    CreateIdBuffer();

    UResourceManager::GetInstance().Initialize(Device,DeviceContext);

    // Initialize Direct2D overlay after device/swapchain ready
    UStatsOverlayD2D::Get().Initialize(Device, DeviceContext, SwapChain);
}

void D3D11RHI::Release()
{
    if (DeviceContext)
    {
        // 파이프라인에서 바인딩된 상태/리소스를 명시적으로 해제
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
        DeviceContext->OMSetDepthStencilState(nullptr, 0);
        DeviceContext->RSSetState(nullptr);
        DeviceContext->OMSetBlendState(nullptr, nullptr, 0xffffffff);

        DeviceContext->ClearState();
        DeviceContext->Flush();
    }

    ReleaseSamplerState();

    // 상수버퍼
    CBUFFER_TYPE_LIST(RELEASE_CBUFFER)


    // 상태 객체
    if (DepthStencilState) { DepthStencilState->Release(); DepthStencilState = nullptr; }
    if (DepthStencilStateLessEqualWrite) { DepthStencilStateLessEqualWrite->Release(); DepthStencilStateLessEqualWrite = nullptr; }
    if (DepthStencilStateLessEqualReadOnly) { DepthStencilStateLessEqualReadOnly->Release(); DepthStencilStateLessEqualReadOnly = nullptr; }
    if (DepthStencilStateAlwaysNoWrite) { DepthStencilStateAlwaysNoWrite->Release(); DepthStencilStateAlwaysNoWrite = nullptr; }
    if (DepthStencilStateDisable) { DepthStencilStateDisable->Release(); DepthStencilStateDisable = nullptr; }
    if (DepthStencilStateGreaterEqualWrite) { DepthStencilStateGreaterEqualWrite->Release(); DepthStencilStateGreaterEqualWrite = nullptr; }

    if (DefaultRasterizerState) { DefaultRasterizerState->Release();   DefaultRasterizerState = nullptr; }
    if (WireFrameRasterizerState) { WireFrameRasterizerState->Release();   WireFrameRasterizerState = nullptr; }
    if (NoCullRasterizerState) { NoCullRasterizerState->Release();   NoCullRasterizerState = nullptr; }
    if (FrontCullRasterizerState) { FrontCullRasterizerState->Release();   FrontCullRasterizerState = nullptr; }
    if (DecalRasterizerState) { DecalRasterizerState->Release();   DecalRasterizerState = nullptr; }

    ReleaseIdBuffer();
    ReleaseBlendState();
    // RTV/DSV/FrameBuffer
    ReleaseFrameBuffer();

    // Device + SwapChain
    ReleaseDeviceAndSwapChain();
}

void D3D11RHI::ClearBackBuffer()
{
    float ClearColor[4] = { 0.025f, 0.025f, 0.025f, 1.0f };
    DeviceContext->ClearRenderTargetView(FrameRTV, ClearColor);
    float IDColor[4] = { 0.0f,0.0f,0.0f,0.0f };
    DeviceContext->ClearRenderTargetView(IdBufferRTV, IDColor);
}

void D3D11RHI::ClearDepthBuffer(float Depth, UINT Stencil)
{
    DeviceContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, Depth, Stencil);

}

void D3D11RHI::CreateBlendState()
{
    D3D11_BLEND_DESC BlendDesc = {};
 
    BlendDesc.IndependentBlendEnable = TRUE;

    D3D11_RENDER_TARGET_BLEND_DESC& Rt0 = BlendDesc.RenderTarget[0];

    Rt0.BlendEnable = TRUE;
    Rt0.SrcBlend = D3D11_BLEND_SRC_ALPHA;
    Rt0.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    Rt0.BlendOp = D3D11_BLEND_OP_ADD;
    Rt0.SrcBlendAlpha = D3D11_BLEND_ONE;
    Rt0.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
    Rt0.BlendOpAlpha = D3D11_BLEND_OP_ADD;
    Rt0.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;


    D3D11_RENDER_TARGET_BLEND_DESC& Rt1 = BlendDesc.RenderTarget[1];
    Rt1.BlendEnable = FALSE;
    Rt1.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Device->CreateBlendState(&BlendDesc, &BlendStateTransparent);

    BlendDesc = {};
    BlendDesc.IndependentBlendEnable = TRUE;
    Rt0 = BlendDesc.RenderTarget[0];
    Rt0.BlendEnable = FALSE;
    Rt0.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

    Rt1 = BlendDesc.RenderTarget[1];
    Rt1.BlendEnable = FALSE;
    Rt1.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    Device->CreateBlendState(&BlendDesc, &BlendStateOpaque);
}

void D3D11RHI::CreateDepthStencilState()
{
    D3D11_DEPTH_STENCIL_DESC desc = {};
    desc.StencilEnable = FALSE;

    // 1) 기본: LessEqual + Write ALL
    desc.DepthEnable = TRUE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
    Device->CreateDepthStencilState(&desc, &DepthStencilStateLessEqualWrite);

    // 2) ReadOnly: LessEqual + Write ZERO
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
    Device->CreateDepthStencilState(&desc, &DepthStencilStateLessEqualReadOnly);

    // 3) AlwaysNoWrite: Always + Write ZERO (기즈모/오버레이 용)
    desc.DepthFunc = D3D11_COMPARISON_ALWAYS;
    // DepthEnable은 TRUE 유지 (읽기 의미는 없지만 상태 일관성을 위해)
    Device->CreateDepthStencilState(&desc, &DepthStencilStateAlwaysNoWrite);

    // 4) Disable: DepthEnable FALSE (테스트/쓰기 모두 무시)
    desc.DepthEnable = FALSE;
    // DepthWriteMask/Func는 무시되지만 값은 그대로 둬도 됨
    Device->CreateDepthStencilState(&desc, &DepthStencilStateDisable);

    // 5) (선택) GreaterEqual + Write ALL
    desc.DepthEnable = TRUE;
    desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    desc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
    Device->CreateDepthStencilState(&desc, &DepthStencilStateGreaterEqualWrite);
}

void D3D11RHI::CreateSamplerState()
{
    D3D11_SAMPLER_DESC SampleDesc = {};
    SampleDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    SampleDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
    SampleDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
    SampleDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
    SampleDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
    SampleDesc.MinLOD = 0;
    SampleDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT HR = Device->CreateSamplerState(&SampleDesc, &DefaultSamplerState);
}

void D3D11RHI::CreateIdBuffer()
{

    DXGI_SWAP_CHAIN_DESC SwapDesc;
    SwapChain->GetDesc(&SwapDesc);

    D3D11_TEXTURE2D_DESC TextureDesc{};
    TextureDesc.Format = DXGI_FORMAT_R32_UINT;
    TextureDesc.CPUAccessFlags = 0;
    TextureDesc.Usage = D3D11_USAGE_DEFAULT;
    TextureDesc.Width = SwapDesc.BufferDesc.Width;
    TextureDesc.Height = SwapDesc.BufferDesc.Height;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

    Device->CreateTexture2D(&TextureDesc, nullptr, &IdBuffer);

    TextureDesc.Format = DXGI_FORMAT_R32_UINT;
    TextureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    TextureDesc.Usage = D3D11_USAGE_STAGING;
    TextureDesc.Width = 1;
    TextureDesc.Height = 1;
    TextureDesc.MipLevels = 1;
    TextureDesc.ArraySize = 1;
    TextureDesc.SampleDesc.Count = 1;
    TextureDesc.SampleDesc.Quality = 0;
    TextureDesc.BindFlags = 0;

    Device->CreateTexture2D(&TextureDesc, nullptr, &IdStagingBuffer);
    if (IdBuffer)
    {
        Device->CreateRenderTargetView(IdBuffer, nullptr, &IdBufferRTV);
    }
}

HRESULT D3D11RHI::CreateIndexBuffer(ID3D11Device* device, const FMeshData* meshData, ID3D11Buffer** outBuffer)
{
    if (!meshData || meshData->Indices.empty())
        return E_FAIL;

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint32) * meshData->Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = meshData->Indices.data();

    return device->CreateBuffer(&ibd, &iinitData, outBuffer);
}

HRESULT D3D11RHI::CreateIndexBuffer(ID3D11Device* device, const FStaticMesh* mesh, ID3D11Buffer** outBuffer)
{
    if (!mesh || mesh->Indices.empty())
        return E_FAIL;

    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_DEFAULT;
    ibd.ByteWidth = static_cast<UINT>(sizeof(uint32) * mesh->Indices.size());
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;

    D3D11_SUBRESOURCE_DATA iinitData = {};
    iinitData.pSysMem = mesh->Indices.data();

    return device->CreateBuffer(&ibd, &iinitData, outBuffer);
}

void D3D11RHI::IASetPrimitiveTopology()
{
    DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void D3D11RHI::RSSetState(EViewModeIndex ViewModeIndex)
{
    if (ViewModeIndex == EViewModeIndex::VMI_Wireframe)
    {
        DeviceContext->RSSetState(WireFrameRasterizerState);
    }
    else
    {
        DeviceContext->RSSetState(DefaultRasterizerState);
    }
}

void D3D11RHI::RSSetFrontCullState()
{
    DeviceContext->RSSetState(FrontCullRasterizerState);
}

void D3D11RHI::RSSetNoCullState()
{
    DeviceContext->RSSetState(NoCullRasterizerState);
}

void D3D11RHI::RSSetDefaultState()
{
    DeviceContext->RSSetState(DefaultRasterizerState);
}

void D3D11RHI::RSSetDecalState()
{
    DeviceContext->RSSetState(DecalRasterizerState);
}

void D3D11RHI::RSSetViewport()
{
    DeviceContext->RSSetViewports(1, &ViewportInfo);
}

void D3D11RHI::OMSetRenderTargets()
{
    ID3D11RenderTargetView* RTVList[]{ FrameRTV, IdBufferRTV };

    DeviceContext->OMSetRenderTargets(2, RTVList, DepthStencilView);
}

void D3D11RHI::OMSetBlendState(bool bIsBlendMode)
{
    if (bIsBlendMode == true)
    {
        DeviceContext->OMSetBlendState(BlendStateTransparent, nullptr, 0xffffffff);
    }
    else
    {
        DeviceContext->OMSetBlendState(BlendStateOpaque, nullptr, 0xffffffff);
    }
}

void D3D11RHI::Present()
{
    // Draw any Direct2D overlays before present
    UStatsOverlayD2D::Get().Draw();
    SwapChain->Present(1, 0); // vsync on
}

void D3D11RHI::CreateDeviceAndSwapChain(HWND hWindow)
{
    // 지원하는 Direct3D 기능 레벨을 정의
    D3D_FEATURE_LEVEL featurelevels[] = { D3D_FEATURE_LEVEL_11_0 };

    // 스왑 체인 설정 구조체 초기화
    DXGI_SWAP_CHAIN_DESC swapchaindesc = {};
    swapchaindesc.BufferDesc.Width = 0; // 창 크기에 맞게 자동으로 설정
    swapchaindesc.BufferDesc.Height = 0; // 창 크기에 맞게 자동으로 설정
    swapchaindesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // 색상 포맷
    swapchaindesc.SampleDesc.Count = 1; // 멀티 샘플링 비활성화
    swapchaindesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 렌더 타겟으로 사용
    swapchaindesc.BufferCount = 2; // 더블 버퍼링
    swapchaindesc.OutputWindow = hWindow; // 렌더링할 창 핸들
    swapchaindesc.Windowed = TRUE; // 창 모드
    swapchaindesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // 스왑 방식

    // Direct3D 장치와 스왑 체인을 생성
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#ifdef _DEBUG
    createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

    HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        createDeviceFlags,
        featurelevels, ARRAYSIZE(featurelevels), D3D11_SDK_VERSION,
        &swapchaindesc, &SwapChain, &Device, nullptr, &DeviceContext);
    // 생성된 스왑 체인의 정보 가져오기
    SwapChain->GetDesc(&swapchaindesc);

    // 뷰포트 정보 설정
    ViewportInfo = { 0.0f, 0.0f, (float)swapchaindesc.BufferDesc.Width, (float)swapchaindesc.BufferDesc.Height, 0.0f, 1.0f };
}

void D3D11RHI::CreateFrameBuffer()
{
    // 백 버퍼 가져오기
    SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&FrameBuffer);

    // 렌더 타겟 뷰 생성
    D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
    framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

    Device->CreateRenderTargetView(FrameBuffer, &framebufferRTVdesc, &FrameRTV);

    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    Device->CreateShaderResourceView(FrameBuffer, &SRVDesc, &FrameSRV);

    // =====================================
    // 깊이/스텐실 버퍼 생성
    // =====================================
    DXGI_SWAP_CHAIN_DESC swapDesc;
    SwapChain->GetDesc(&swapDesc);

    D3D11_TEXTURE2D_DESC depthDesc = {};
    depthDesc.Width = swapDesc.BufferDesc.Width;
    depthDesc.Height = swapDesc.BufferDesc.Height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // SRV를 생성하려면 TYPELESS 포맷 사용
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE; // SRV 바인딩 추가

    ID3D11Texture2D* depthBuffer = nullptr;
    Device->CreateTexture2D(&depthDesc, nullptr, &depthBuffer);

    // DepthStencilView 생성
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DSV는 D24_UNORM_S8_UINT 포맷
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    Device->CreateDepthStencilView(depthBuffer, &dsvDesc, &DepthStencilView);

    // ShaderResourceView 생성 (데칼 렌더링에서 사용)
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS; // SRV는 R24_UNORM_X8_TYPELESS 포맷
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;

    Device->CreateShaderResourceView(depthBuffer, &srvDesc, &DepthSRV);

    depthBuffer->Release(); // 뷰만 참조 유지
}

void D3D11RHI::CreateRasterizerState()
{
    // 이미 생성된 경우 중복 생성 방지
    if (DefaultRasterizerState)
        return;

    D3D11_RASTERIZER_DESC deafultrasterizerdesc = {};
    deafultrasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
    deafultrasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링
    deafultrasterizerdesc.DepthClipEnable = TRUE; // 근/원거리 평면 클리핑

    Device->CreateRasterizerState(&deafultrasterizerdesc, &DefaultRasterizerState);

    D3D11_RASTERIZER_DESC wireframerasterizerdesc = {};
    wireframerasterizerdesc.FillMode = D3D11_FILL_WIREFRAME; // 채우기 모드
    wireframerasterizerdesc.CullMode = D3D11_CULL_BACK; // 백 페이스 컬링
    wireframerasterizerdesc.DepthClipEnable = TRUE; // 근/원거리 평면 클리핑

    Device->CreateRasterizerState(&wireframerasterizerdesc, &WireFrameRasterizerState);

    D3D11_RASTERIZER_DESC frontcullrasterizerdesc = {};
    frontcullrasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
    frontcullrasterizerdesc.CullMode = D3D11_CULL_FRONT; // 프론트 페이스 컬링
    frontcullrasterizerdesc.DepthClipEnable = TRUE; // 근/원거리 평면 클리핑

    Device->CreateRasterizerState(&frontcullrasterizerdesc, &FrontCullRasterizerState);

    D3D11_RASTERIZER_DESC nocullrasterizerdesc = {};
    nocullrasterizerdesc.FillMode = D3D11_FILL_SOLID; // 채우기 모드
    nocullrasterizerdesc.CullMode = D3D11_CULL_NONE; // 컬링 없음
    nocullrasterizerdesc.DepthClipEnable = TRUE; // 근/원거리 평면 클리핑

    Device->CreateRasterizerState(&nocullrasterizerdesc, &NoCullRasterizerState);

    // 데칼용 DepthBias 적용된 state
    D3D11_RASTERIZER_DESC decalrasterizerdesc = {};
    decalrasterizerdesc.FillMode = D3D11_FILL_SOLID;
    decalrasterizerdesc.CullMode = D3D11_CULL_NONE;
    decalrasterizerdesc.DepthClipEnable = TRUE;
    decalrasterizerdesc.DepthBias = -100; // z-fighting 방지용 bias
    decalrasterizerdesc.DepthBiasClamp = 0.0f;
    decalrasterizerdesc.SlopeScaledDepthBias = -1.0f;

    Device->CreateRasterizerState(&decalrasterizerdesc, &DecalRasterizerState);
}

void D3D11RHI::CreateConstantBuffer()
{
    CBUFFER_TYPE_LIST(CREATE_CBUFFER)
}

void D3D11RHI::ReleaseSamplerState()
{
    if (DefaultSamplerState)
    {
        DefaultSamplerState->Release();
        DefaultSamplerState = nullptr;
	}
}

void D3D11RHI::ReleaseBlendState()
{
    if (BlendStateOpaque) { BlendStateOpaque->Release();        BlendStateOpaque = nullptr; }
    if (BlendStateTransparent) { BlendStateTransparent->Release();  BlendStateTransparent = nullptr; }
}

void D3D11RHI::ReleaseRasterizerState()
{
    if (DefaultRasterizerState)
    {
        DefaultRasterizerState->Release();
        DefaultRasterizerState = nullptr;
    }
    if (WireFrameRasterizerState)
    {
        WireFrameRasterizerState->Release();
        WireFrameRasterizerState = nullptr;
    }
    if (FrontCullRasterizerState)
    {
        FrontCullRasterizerState->Release();
        FrontCullRasterizerState = nullptr;
    }
    if (NoCullRasterizerState)
    {
        NoCullRasterizerState->Release();
        NoCullRasterizerState = nullptr;
    }
    if (DecalRasterizerState)
    {
        DecalRasterizerState->Release();
        DecalRasterizerState = nullptr;
    }
    DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
}

void D3D11RHI::ReleaseFrameBuffer()
{
    if (FrameBuffer)
    {
        FrameBuffer->Release();
        FrameBuffer = nullptr;
    }
    if (FrameRTV)
    {
        FrameRTV->Release();
        FrameRTV = nullptr;
    }

    if (DepthSRV)
    {
        DepthSRV->Release();
        DepthSRV = nullptr;
    }

    if (DepthStencilView)
    {
        DepthStencilView->Release();
        DepthStencilView = nullptr;
    }
}

void D3D11RHI::ReleaseDeviceAndSwapChain()
{
    if (SwapChain)
    {
        SwapChain->Release();
        SwapChain = nullptr;
    }

    if (DeviceContext)
    {
        DeviceContext->Release();
        DeviceContext = nullptr;
    }

    if (Device)
    {
        Device->Release();
        Device = nullptr;
    }

}

void D3D11RHI::ReleaseIdBuffer()
{
    if (IdBufferRTV)
    {
        IdBufferRTV->Release();
        IdBufferRTV = nullptr;
    }
    if (IdStagingBuffer)
    {
        IdStagingBuffer->Release();
        IdStagingBuffer = nullptr;
    }
    if (IdBuffer)
    {
        IdBuffer->Release();
        IdBuffer = nullptr;
    }
}

void D3D11RHI::OmSetDepthStencilState(EComparisonFunc Func)
{
    switch (Func)
    {
    case EComparisonFunc::Always:
        DeviceContext->OMSetDepthStencilState(DepthStencilStateAlwaysNoWrite, 0);
        break;
    case EComparisonFunc::LessEqual:
        DeviceContext->OMSetDepthStencilState(DepthStencilStateLessEqualWrite, 0);
        break;
    case EComparisonFunc::LessEqualReadOnly:
        DeviceContext->OMSetDepthStencilState(DepthStencilStateLessEqualReadOnly, 0);
        break;
    case EComparisonFunc::GreaterEqual:
        DeviceContext->OMSetDepthStencilState(DepthStencilStateGreaterEqualWrite, 0);
        break;
    case EComparisonFunc::Disable:
        DeviceContext->OMSetDepthStencilState(DepthStencilStateDisable, 0);
    }
}
void D3D11RHI::CreateBackBufferAndDepthStencil(UINT width, UINT height)
{
    // 기존 바인딩 해제 후 뷰 해제
    if (FrameRTV) { DeviceContext->OMSetRenderTargets(0, nullptr, nullptr); FrameRTV->Release(); FrameRTV = nullptr; }
    if (FrameSRV) { FrameSRV->Release(); FrameSRV = nullptr; }
    if (DepthStencilView) { DepthStencilView->Release(); DepthStencilView = nullptr; }

    // 1) 백버퍼에서 RTV 생성
    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
    if (FAILED(hr) || !backBuffer) {
        UE_LOG("GetBuffer(0) failed.\n");
        return;
    }

    D3D11_RENDER_TARGET_VIEW_DESC framebufferRTVdesc = {};
    framebufferRTVdesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM_SRGB;
    framebufferRTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    hr = Device->CreateRenderTargetView(backBuffer, &framebufferRTVdesc, &FrameRTV);
    if (FAILED(hr) || !FrameRTV) {
        UE_LOG("CreateRenderTargetView failed.\n");
        return;
    }
    D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc = {};
    SRVDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
    SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    hr = Device->CreateShaderResourceView(backBuffer, &SRVDesc, &FrameSRV);
    backBuffer->Release();


    // 2) DepthStencil 텍스처/뷰 생성
    ID3D11Texture2D* depthTex = nullptr;
    D3D11_TEXTURE2D_DESC depthDesc{};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;               // 멀티샘플링 끄는 경우
    depthDesc.SampleDesc.Quality = 0;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;

    hr = Device->CreateTexture2D(&depthDesc, nullptr, &depthTex);
    if (FAILED(hr) || !depthTex) {
        UE_LOG("CreateTexture2D(depth) failed.\n");
        return;
    }

    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Format = depthDesc.Format;
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = Device->CreateDepthStencilView(depthTex, &dsvDesc, &DepthStencilView);
    depthTex->Release();
    if (FAILED(hr) || !DepthStencilView) {
        UE_LOG("CreateDepthStencilView failed.\n");
        return;
    }

    // 3) OM 바인딩
    DeviceContext->OMSetRenderTargets(1, &FrameRTV, DepthStencilView);

    // 4) 뷰포트 갱신
    SetViewport(width, height);
}

// ──────────────────────────────────────────────────────
// Helper: Viewport 갱신
// ──────────────────────────────────────────────────────
void D3D11RHI::SetViewport(UINT width, UINT height)
{
    ViewportInfo.TopLeftX = 0.0f;
    ViewportInfo.TopLeftY = 0.0f;
    ViewportInfo.Width = static_cast<float>(width);
    ViewportInfo.Height = static_cast<float>(height);
    ViewportInfo.MinDepth = 0.0f;
    ViewportInfo.MaxDepth = 1.0f;

    DeviceContext->RSSetViewports(1, &ViewportInfo);
}

// ──────────────────────────────────────────────────────
// 기존 오타 호출 호환용 래퍼 (선택)
// ──────────────────────────────────────────────────────
void D3D11RHI::setviewort(UINT width, UINT height)
{
    SetViewport(width, height);
}
void D3D11RHI::ResizeSwapChain(UINT width, UINT height)
{
    if (!SwapChain) return;

    // 렌더링 완료까지 대기 (중요!)
    if (DeviceContext) {
        DeviceContext->Flush();
    }

    // 현재 렌더 타겟 언바인딩
    if (DeviceContext) {
        DeviceContext->OMSetRenderTargets(0, nullptr, nullptr);
    }
    ReleaseIdBuffer();
    // 기존 뷰 해제
    if (FrameRTV) { FrameRTV->Release(); FrameRTV = nullptr; }
    if (FrameSRV) { FrameSRV->Release(); FrameSRV = nullptr; }
    if (DepthStencilView) { DepthStencilView->Release(); DepthStencilView = nullptr; }
    if (FrameBuffer) { FrameBuffer->Release(); FrameBuffer = nullptr; }

    // 스왑체인 버퍼 리사이즈
    HRESULT hr = SwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);
    if (FAILED(hr)) { UE_LOG("ResizeBuffers failed!\n"); return; }

    // 다시 RTV/DSV 만들기
    CreateBackBufferAndDepthStencil(width, height);
    CreateIdBuffer();
    // 뷰포트도 갱신
    setviewort(width, height);
}

void D3D11RHI::PSSetDefaultSampler(UINT StartSlot)
{
	DeviceContext->PSSetSamplers(StartSlot, 1, &DefaultSamplerState);
}