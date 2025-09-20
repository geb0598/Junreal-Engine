#include "pch.h"
#include "MultiViewportWindow.h"
#include "FViewport.h"
#include "FViewportClient.h"

MultiViewportWindow::MultiViewportWindow()
{
}

MultiViewportWindow::~MultiViewportWindow()
{
    Cleanup();
}

bool MultiViewportWindow::Initialize(uint32 InTotalWidth, uint32 InTotalHeight, ID3D11Device* Device)
{
    if (!Device)
        return false;

    D3DDevice = Device;
    D3DDevice->GetImmediateContext(&D3DDeviceContext);

    TotalWidth = InTotalWidth;
    TotalHeight = InTotalHeight;

    UpdateViewportSizes();
    CreateViewports();

    return IsValid();
}

void MultiViewportWindow::Cleanup()
{
    ReleaseViewports();

    if (MainRenderTargetView)
    {
        MainRenderTargetView->Release();
        MainRenderTargetView = nullptr;
    }

    if (D3DDeviceContext)
    {
        D3DDeviceContext->Release();
        D3DDeviceContext = nullptr;
    }

    MainSwapChain = nullptr;
    D3DDevice = nullptr;
}

void MultiViewportWindow::Resize(uint32 NewTotalWidth, uint32 NewTotalHeight)
{
    TotalWidth = NewTotalWidth;
    TotalHeight = NewTotalHeight;

    UpdateViewportSizes();

    // 각 뷰포트 크기 조정
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        if (Viewports[i])
        {
            Viewports[i]->Resize(ViewportWidth, ViewportHeight);
        }
    }
}

FViewport* MultiViewportWindow::GetViewport(EMultiViewportType ViewportType)
{
    int32 Index = static_cast<int32>(ViewportType);
    if (Index >= 0 && Index < static_cast<int32>(EMultiViewportType::MAX))
    {
        return Viewports[Index];
    }
    return nullptr;
}

const FViewport* MultiViewportWindow::GetViewport(EMultiViewportType ViewportType) const
{
    int32 Index = static_cast<int32>(ViewportType);
    if (Index >= 0 && Index < static_cast<int32>(EMultiViewportType::MAX))
    {
        return Viewports[Index];
    }
    return nullptr;
}

void MultiViewportWindow::BeginRenderFrame()
{
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        if (Viewports[i] && Viewports[i]->IsValid())
        {
            Viewports[i]->BeginRenderFrame();
        }
    }
}

void MultiViewportWindow::EndRenderFrame()
{
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        if (Viewports[i] && Viewports[i]->IsValid())
        {
            Viewports[i]->EndRenderFrame();
        }
    }
}

void MultiViewportWindow::Present()
{
    if (!D3DDeviceContext || !MainSwapChain)
        return;

    // 메인 백버퍼 가져오기
    if (!MainRenderTargetView)
    {
        ID3D11Texture2D* BackBuffer = nullptr;
        MainSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&BackBuffer);
        if (BackBuffer)
        {
            D3DDevice->CreateRenderTargetView(BackBuffer, nullptr, &MainRenderTargetView);
            BackBuffer->Release();
        }
    }

    if (!MainRenderTargetView)
        return;

    // 메인 백버퍼를 렌더 타겟으로 설정
    D3DDeviceContext->OMSetRenderTargets(1, &MainRenderTargetView, nullptr);

    // 메인 뷰포트 설정
    D3D11_VIEWPORT mainViewport = {};
    mainViewport.Width = static_cast<float>(TotalWidth);
    mainViewport.Height = static_cast<float>(TotalHeight);
    mainViewport.MinDepth = 0.0f;
    mainViewport.MaxDepth = 1.0f;
    mainViewport.TopLeftX = 0.0f;
    mainViewport.TopLeftY = 0.0f;
    D3DDeviceContext->RSSetViewports(1, &mainViewport);

    // 백버퍼 클리어
    float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
    D3DDeviceContext->ClearRenderTargetView(MainRenderTargetView, ClearColor);

    // 각 뷰포트의 렌더 타겟을 메인 백버퍼에 복사
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        if (Viewports[i] && Viewports[i]->IsValid())
        {
            CopyViewportToBackBuffer(static_cast<EMultiViewportType>(i), Viewports[i]);
        }
    }

    // 스왑체인 Present
    MainSwapChain->Present(0, 0);
}

FVector2D MultiViewportWindow::GetViewportPosition(EMultiViewportType ViewportType) const
{
    switch (ViewportType)
    {
    case EMultiViewportType::TopLeft:
        return FVector2D(0, 0);
    case EMultiViewportType::TopRight:
        return FVector2D(ViewportWidth, 0);
    case EMultiViewportType::BottomLeft:
        return FVector2D(0, ViewportHeight);
    case EMultiViewportType::BottomRight:
        return FVector2D(ViewportWidth, ViewportHeight);
    default:
        return FVector2D(0, 0);
    }
}

FVector2D MultiViewportWindow::GetViewportSize() const
{
    return FVector2D(ViewportWidth, ViewportHeight);
}

bool MultiViewportWindow::GetViewportFromScreenCoords(int32 ScreenX, int32 ScreenY, EMultiViewportType& OutViewportType, int32& OutLocalX, int32& OutLocalY) const
{
    // 화면 좌표를 뷰포트 영역으로 변환
    bool bIsLeft = ScreenX < static_cast<int32>(ViewportWidth);
    bool bIsTop = ScreenY < static_cast<int32>(ViewportHeight);

    if (bIsLeft && bIsTop)
    {
        OutViewportType = EMultiViewportType::TopLeft;
        OutLocalX = ScreenX;
        OutLocalY = ScreenY;
    }
    else if (!bIsLeft && bIsTop)
    {
        OutViewportType = EMultiViewportType::TopRight;
        OutLocalX = ScreenX - ViewportWidth;
        OutLocalY = ScreenY;
    }
    else if (bIsLeft && !bIsTop)
    {
        OutViewportType = EMultiViewportType::BottomLeft;
        OutLocalX = ScreenX;
        OutLocalY = ScreenY - ViewportHeight;
    }
    else
    {
        OutViewportType = EMultiViewportType::BottomRight;
        OutLocalX = ScreenX - ViewportWidth;
        OutLocalY = ScreenY - ViewportHeight;
    }

    return true;
}

bool MultiViewportWindow::IsValid() const
{
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        if (!Viewports[i] || !Viewports[i]->IsValid())
        {
            return false;
        }
    }
    return true;
}

void MultiViewportWindow::CreateViewports()
{
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        // FViewport 생성
        Viewports[i] = new FViewport();
        if (!Viewports[i]->Initialize(ViewportWidth, ViewportHeight, D3DDevice))
        {
            delete Viewports[i];
            Viewports[i] = nullptr;
            continue;
        }

        // FViewportClient 생성 및 설정
        ViewportClients[i] = new FViewportClient();
        Viewports[i]->SetViewportClient(ViewportClients[i]);
    }
}

void MultiViewportWindow::ReleaseViewports()
{
    for (int32 i = 0; i < static_cast<int32>(EMultiViewportType::MAX); ++i)
    {
        if (Viewports[i])
        {
            Viewports[i]->Cleanup();
            delete Viewports[i];
            Viewports[i] = nullptr;
        }

        if (ViewportClients[i])
        {
            delete ViewportClients[i];
            ViewportClients[i] = nullptr;
        }
    }
}

void MultiViewportWindow::UpdateViewportSizes()
{
    // 전체 화면을 4등분
    ViewportWidth = TotalWidth / 2;
    ViewportHeight = TotalHeight / 2;

    // 최소 크기 보장
    if (ViewportWidth == 0) ViewportWidth = 1;
    if (ViewportHeight == 0) ViewportHeight = 1;
}

void MultiViewportWindow::CopyViewportToBackBuffer(EMultiViewportType ViewportType, FViewport* Viewport)
{
    if (!Viewport || !Viewport->IsValid() || !D3DDeviceContext)
        return;

    // 각 뷰포트 영역에 다른 색상으로 테스트 렌더링
    FVector2D Position = GetViewportPosition(ViewportType);

    // 해당 뷰포트 영역 설정
    D3D11_VIEWPORT quadViewport = {};
    quadViewport.Width = static_cast<float>(ViewportWidth);
    quadViewport.Height = static_cast<float>(ViewportHeight);
    quadViewport.MinDepth = 0.0f;
    quadViewport.MaxDepth = 1.0f;
    quadViewport.TopLeftX = Position.X;
    quadViewport.TopLeftY = Position.Y;
    D3DDeviceContext->RSSetViewports(1, &quadViewport);

    // 해당 영역을 뷰포트 타입별 색상으로 클리어 (테스트용)
    float Colors[4][4] = {
        { 0.5f, 0.2f, 0.2f, 1.0f }, // TopLeft - 빨강
        { 0.2f, 0.5f, 0.2f, 1.0f }, // TopRight - 초록
        { 0.2f, 0.2f, 0.5f, 1.0f }, // BottomLeft - 파랑
        { 0.5f, 0.5f, 0.2f, 1.0f }  // BottomRight - 노랑
    };

    int Index = static_cast<int>(ViewportType);
    D3DDeviceContext->ClearRenderTargetView(MainRenderTargetView, Colors[Index]);
}