#include "pch.h"
#include "SViewportWindow.h"
#include "World.h"
extern float CLIENTWIDTH;
extern float CLIENTHEIGHT;
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
    case EViewportType::Orthographic_Left:  ViewportName = "Left"; break;
    case EViewportType::Orthographic_Top:   ViewportName = "Top"; break;
    case EViewportType::Orthographic_Back: ViewportName = "Back"; break;
    case EViewportType::Orthographic_Right:  ViewportName = "Right"; break;
    case EViewportType::Orthographic_Bottom:   ViewportName = "Bottom"; break;
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
 /*   ImDrawList* DrawList = ImGui::GetBackgroundDrawList();
    DrawList->AddText(
        ImVec2(Rect.Min.X + 5, Rect.Min.Y + 5),
        IM_COL32(255, 255, 255, 255),
        ViewportName.c_str()
    );*/
}

void SViewportWindow::OnUpdate()
{
    if (!Viewport)
        return;

    if (!Viewport) return;

    uint32 NewStartX = static_cast<uint32>(Rect.Left);
    uint32 NewStartY = static_cast<uint32>(Rect.Top);
    uint32 NewWidth = static_cast<uint32>(CLIENTWIDTH);
    uint32 NewHeight = static_cast<uint32>(CLIENTHEIGHT);

    Viewport->Resize(NewStartX, NewStartY, NewWidth, NewHeight);
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

    // FViewport의 ProcessMouseButtonDown을 통해 ViewportClient로 이벤트 전달
    Viewport->ProcessMouseButtonDown((int32)LocalPos.X, (int32)LocalPos.Y, 0);
}

void SViewportWindow::OnMouseUp(FVector2D MousePos)
{
    if (!Viewport) return;

    bIsMouseDown = false;
    FVector2D LocalPos = MousePos - Rect.Min;
    Viewport->ProcessMouseButtonUp((int32)LocalPos.X, (int32)LocalPos.Y, 0);
}