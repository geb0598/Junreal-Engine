#include "pch.h"
#include "SMultiViewportWindow.h"
#include "SWindow.h"
#include "SSplitterV.h"
#include "ImGui/imgui.h"
extern float CLIENTWIDTH;
extern float CLIENTHEIGHT;
SMultiViewportWindow::SMultiViewportWindow()
{
    for (int i = 0; i < 4; i++)
        Viewports[i] = nullptr;
}

SMultiViewportWindow::~SMultiViewportWindow()
{
    delete RootSplitter;
    for (int i = 0; i < 4; i++)
        delete Viewports[i];
}

void SMultiViewportWindow::Initialize(ID3D11Device* Device, UWorld* World,const FRect& InRect)
{
    Rect = InRect;

    // 최상위: 수직 스플리터
    RootSplitter = new SSplitterV();
    RootSplitter->SetRect(Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y);

    // 좌/우 영역에 수평 스플리터 배치
    SSplitterH* Left = new SSplitterH();
    SSplitterH* Right = new SSplitterH();
    RootSplitter->SideLT = Left;
    RootSplitter->SideRB = Right;

    // 뷰포트 생성
    Viewports[0] = new SViewportWindow();
    Viewports[1] = new SViewportWindow();
    Viewports[2] = new SViewportWindow();
    Viewports[3] = new SViewportWindow();
    Viewports[0]->Initialize(0, 0,
        Rect.GetWidth() / 2, Rect.GetHeight() / 2,
        World, Device, EViewportType::Perspective);
    Viewports[1]->Initialize(Rect.GetWidth() / 2, 0,
        Rect.GetWidth(), Rect.GetHeight() / 2,
        World, Device, EViewportType::Orthographic_Front);

    Viewports[2]->Initialize(0, Rect.GetHeight() / 2,
        Rect.GetWidth() / 2, Rect.GetHeight() ,
        World, Device, EViewportType::Orthographic_Left);

    Viewports[3]->Initialize(Rect.GetWidth() / 2, Rect.GetHeight() / 2,
        Rect.GetWidth(), Rect.GetHeight(),
        World, Device, EViewportType::Orthographic_Top);

    // 스플리터에 연결
    Left->SideLT = Viewports[0];
    Left->SideRB = Viewports[1];
    Right->SideLT = Viewports[2];
    Right->SideRB = Viewports[3];
}

void SMultiViewportWindow::OnRender()
{
    if (RootSplitter)
    {
        RootSplitter->OnRender();

        // 뷰포트 간 경계선을 더 명확하게 표시
        ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

        // 수직선 (중앙)
        float centerX = Rect.Min.X + (Rect.Max.X - Rect.Min.X) * 0.5f;
        DrawList->AddLine(
            ImVec2(centerX, Rect.Min.Y),
            ImVec2(centerX, Rect.Max.Y),
            IM_COL32(100, 100, 100, 255), 2.0f
        );

        // 수평선 (중앙)
        float centerY = Rect.Min.Y + (Rect.Max.Y - Rect.Min.Y) * 0.5f;
        DrawList->AddLine(
            ImVec2(Rect.Min.X, centerY),
            ImVec2(Rect.Max.X, centerY),
            IM_COL32(100, 100, 100, 255), 2.0f
        );
    }
}

void SMultiViewportWindow::OnUpdate()
{
    if (RootSplitter) {
        RootSplitter->Rect = FRect(0, 0, CLIENTWIDTH, CLIENTHEIGHT);
        RootSplitter->OnUpdate();
    }
        
}

void SMultiViewportWindow::OnMouseMove(FVector2D MousePos)
{
    if (RootSplitter)
        RootSplitter->OnMouseMove(MousePos);
}

void SMultiViewportWindow::OnMouseDown(FVector2D MousePos)
{
    if (RootSplitter)
        RootSplitter->OnMouseDown(MousePos);
}

void SMultiViewportWindow::OnMouseUp(FVector2D MousePos)
{
    if (RootSplitter)
        RootSplitter->OnMouseUp(MousePos);
}