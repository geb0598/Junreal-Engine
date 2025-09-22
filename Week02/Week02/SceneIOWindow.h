#pragma once
#include "SWindow.h"
#include "ImGui/imgui.h"

class SSceneIOWindow : public SWindow
{
public:
    void OnRender() override
    {
        // 우측 패널 고정 폭 예시
        ImGui::SetNextWindowPos(ImVec2(Rect.Min.X, Rect.Min.Y));
        ImGui::SetNextWindowSize(ImVec2(Rect.GetWidth(), Rect.GetHeight()));

        ImGuiWindowFlags flags = ImGuiWindowFlags_NoCollapse
            | ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_NoMove;

        if (ImGui::Begin("Scene IO", nullptr, flags))
        {
            ImGui::Text("Scene Tools");
            ImGui::Separator();

            if (ImGui::Button("Save Scene", ImVec2(120, 30)))
            {
                // TODO: 저장 로직 연결
                // 예: World->SaveScene("scene.json");
            }

            if (ImGui::Button("Load Scene", ImVec2(120, 30)))
            {
                // TODO: 불러오기 로직 연결
                // 예: World->LoadScene("scene.json");
            }

            ImGui::Separator();
            ImGui::Text("Experimental Features");
       //     ImGui::Checkbox("Auto Save", nullptr); // 단순 예시
        }
        ImGui::End();
    }
};