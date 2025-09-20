#pragma once
#include "Object.h"
#include "Vector.h"
#include <d3d11.h>
#include <dxgi.h>

class FViewport;
class FViewportClient;

/**
 * @brief 멀티 뷰포트 타입 열거형
 */
enum class EMultiViewportType : uint32
{
    TopLeft = 0,        // 좌상단 (Perspective)
    TopRight = 1,       // 우상단 (Front)
    BottomLeft = 2,     // 좌하단 (Side)
    BottomRight = 3,    // 우하단 (Top)
    MAX
};

/**
 * @brief 4분할 뷰포트를 관리하는 클래스
 */
class MultiViewportWindow
{
public:
    MultiViewportWindow();
    virtual ~MultiViewportWindow();

    // 초기화 및 정리
    bool Initialize(uint32 InTotalWidth, uint32 InTotalHeight, ID3D11Device* Device);
    void Cleanup();

    // 메인 스왑체인 설정
    void SetMainSwapChain(IDXGISwapChain* InSwapChain) { MainSwapChain = InSwapChain; }

    // 크기 조정
    void Resize(uint32 NewTotalWidth, uint32 NewTotalHeight);

    // 뷰포트 접근
    FViewport* GetViewport(EMultiViewportType ViewportType);
    const FViewport* GetViewport(EMultiViewportType ViewportType) const;

    // 렌더링
    void BeginRenderFrame();
    void EndRenderFrame();
    void Present();

    // 뷰포트 영역 계산
    FVector2D GetViewportPosition(EMultiViewportType ViewportType) const;
    FVector2D GetViewportSize() const;

    // 마우스 좌표를 뷰포트 타입과 로컬 좌표로 변환
    bool GetViewportFromScreenCoords(int32 ScreenX, int32 ScreenY, EMultiViewportType& OutViewportType, int32& OutLocalX, int32& OutLocalY) const;

    // 유효성 검사
    bool IsValid() const;

    // 접근자
    uint32 GetTotalWidth() const { return TotalWidth; }
    uint32 GetTotalHeight() const { return TotalHeight; }

private:
    void CreateViewports();
    void ReleaseViewports();
    void UpdateViewportSizes();
    void CopyViewportToBackBuffer(EMultiViewportType ViewportType, FViewport* Viewport);

    // 전체 크기
    uint32 TotalWidth = 0;
    uint32 TotalHeight = 0;

    // 개별 뷰포트 크기
    uint32 ViewportWidth = 0;
    uint32 ViewportHeight = 0;

    // D3D 디바이스
    ID3D11Device* D3DDevice = nullptr;
    ID3D11DeviceContext* D3DDeviceContext = nullptr;

    // 메인 스왑체인 (화면 출력용)
    IDXGISwapChain* MainSwapChain = nullptr;
    ID3D11RenderTargetView* MainRenderTargetView = nullptr;

    // 4개의 뷰포트
    FViewport* Viewports[static_cast<int32>(EMultiViewportType::MAX)] = { nullptr };

    // 뷰포트 클라이언트들
    FViewportClient* ViewportClients[static_cast<int32>(EMultiViewportType::MAX)] = { nullptr };
};