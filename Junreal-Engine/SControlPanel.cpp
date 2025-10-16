﻿#include "pch.h"
#include "SControlPanel.h"
#include"UI/Window/ControlPanelWindow.h"
#include"UI/Window/SceneWindow.h"

SControlPanel::SControlPanel()
{
	ControlPanelWidget=new UControlPanelWindow();
    SceneWindow = new USceneWindow();
}

SControlPanel::~SControlPanel()
{
    delete ControlPanelWidget;
    delete SceneWindow;
}

void SControlPanel::OnRender()
{
    ImGui::SetNextWindowPos(ImVec2(Rect.Min.X, Rect.Min.Y));
    ImGui::SetNextWindowSize(ImVec2(Rect.GetWidth(), Rect.GetHeight()));

    ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse
        | ImGuiWindowFlags_NoResize
        | ImGuiWindowFlags_NoMove;

    if (ImGui::Begin("Control", nullptr, flags))
    {
       /* if (SceneWindow) {
            SceneWindow->RenderWidget();
        }*/
        if (ControlPanelWidget)
            ControlPanelWidget->RenderWidget();
    }
    ImGui::End();
	
}

void SControlPanel::OnUpdate(float deltaSecond)
{
    SceneWindow->Update();
    ControlPanelWidget->Update();
}
