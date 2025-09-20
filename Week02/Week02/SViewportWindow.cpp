#include "pch.h"
#include "SViewportWindow.h"
#include "World.h"

SViewportWindow::SViewportWindow()
{
    ViewportType = EViewportType::Perspective;
    bIsActive = false;
    bIsMouseDown = false;
}

SViewportWindow::~SViewportWindow()
{
    if (Viewport)
    {
        delete Viewport;
        Viewport = nullptr;
    }

    if (ViewportClient)
    {
        delete ViewportClient;
        ViewportClient = nullptr;
    }
}

bool SViewportWindow::Initialize(uint32 StartX, uint32 StartY, uint32 Width, uint32 Height, UWorld* World, ID3D11Device* Device, EViewportType InViewportType)
{
    ViewportType = InViewportType;

    // 이름 설정
    switch (ViewportType)
    {
    case EViewportType::Perspective:       ViewportName = "Perspective"; break;
    case EViewportType::Orthographic_Front: ViewportName = "Front"; break;
    case EViewportType::Orthographic_Side:  ViewportName = "Side"; break;
    case EViewportType::Orthographic_Top:   ViewportName = "Top"; break;
    }

    // FViewport 생성
    Viewport = new FViewport();
    if (!Viewport->Initialize(StartX,StartY, Width, Height, Device))
    {
        delete Viewport;
        Viewport = nullptr;
        return false;
    }

    // FViewportClient 생성
    ViewportClient = new FViewportClient();
    ViewportClient->SetViewportType(ViewportType);
    ViewportClient->SetWorld(World); // 전역 월드 연결 (이미 있다고 가정)

    // 양방향 연결
    Viewport->SetViewportClient(ViewportClient);

    return true;
}

void SViewportWindow::OnRender()
{
    if (!Viewport)
        return;

    Viewport->BeginRenderFrame();

    if (ViewportClient)
        ViewportClient->Draw(Viewport);

    Viewport->EndRenderFrame();

    // ImGui를 통한 이름 표시
    ImDrawList* DrawList = ImGui::GetBackgroundDrawList();
    DrawList->AddText(
        ImVec2(Rect.Min.X + 5, Rect.Min.Y + 5),
        IM_COL32(255, 255, 255, 255),
        ViewportName.c_str()
    );
}

void SViewportWindow::OnUpdate()
{
    if (!Viewport)
        return;

    // 뷰포트 크기 갱신
    uint32 NewWidth = static_cast<uint32>(GetWidth());
    uint32 NewHeight = static_cast<uint32>(GetHeight());
    Viewport->Resize(NewWidth, NewHeight);
}

void SViewportWindow::OnMouseMove(FVector2D MousePos)
{
    if (!Viewport) return;

    FVector2D LocalPos = MousePos - Rect.Min; // 윈도우 내부 좌표
    Viewport->ProcessMouseMove((int32)LocalPos.X, (int32)LocalPos.Y);
}

void SViewportWindow::OnMouseDown(FVector2D MousePos)
{
    if (!Viewport) return;

    bIsMouseDown = true;
    FVector2D LocalPos = MousePos - Rect.Min;
    Viewport->ProcessMouseButtonDown((int32)LocalPos.X, (int32)LocalPos.Y, 0);
}

void SViewportWindow::OnMouseUp(FVector2D MousePos)
{
    if (!Viewport) return;

    bIsMouseDown = false;
    FVector2D LocalPos = MousePos - Rect.Min;
    Viewport->ProcessMouseButtonUp((int32)LocalPos.X, (int32)LocalPos.Y, 0);
}