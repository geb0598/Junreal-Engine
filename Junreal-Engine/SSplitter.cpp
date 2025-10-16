﻿#include "pch.h"
#include "SSplitter.h"
extern float CLIENTWIDTH;
extern float CLIENTHEIGHT;
SSplitter::SSplitter()
{
    SplitRatio = 0.5f;
    SplitterThickness = 10;
    bIsDragging = false;
}

SSplitter::~SSplitter()
{
    //// 소유권 명확화: 자식 윈도우 재귀 삭제
    //if (SideLT)
    //{
    //    delete SideLT;
    //    SideLT = nullptr;
    //}
    //if (SideRB)
    //{
    //    delete SideRB;
    //    SideRB = nullptr;
    //}
}

bool SSplitter::IsMouseOnSplitter(FVector2D MousePos) const
{
    FRect SplitterRect = GetSplitterRect();
    return MousePos.X >= SplitterRect.Min.X && MousePos.X <= SplitterRect.Max.X &&
        MousePos.Y >= SplitterRect.Min.Y && MousePos.Y <= SplitterRect.Max.Y;
}

void SSplitter::StartDrag(FVector2D MousePos)
{
    if (IsMouseOnSplitter(MousePos))
    {
        bIsDragging = true;
        DragStartPos = MousePos;
        DragStartRatio = SplitRatio;
    }
}

void SSplitter::UpdateDrag(FVector2D MousePos)
{
    if (!bIsDragging) return;

    // 수평/수직 스플리터별로 다른 계산 필요
    // 하위 클래스에서 오버라이드하여 구체적 구현
}

void SSplitter::EndDrag()
{
    bIsDragging = false;
}

void SSplitter::OnRender()
{
    // 자식 윈도우들 렌더링
    if (SideLT) SideLT->OnRender();
    if (SideRB) SideRB->OnRender();

    // 스플리터 라인 그리기
    FRect SplitterRect = GetSplitterRect();
    ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

    // 드래그 중이거나 마우스 호버 시 색상 변경
    ImU32 SplitterColor;
    if (bIsDragging)
    {
        SplitterColor = ImGui::GetColorU32(ImGuiCol_ButtonActive);
    }
    else if (IsMouseOnSplitter(FVector2D(ImGui::GetMousePos().x, ImGui::GetMousePos().y)))
    {
        SplitterColor = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
    }
    else
    {
        SplitterColor = ImGui::GetColorU32(ImGuiCol_Separator);
    }

    DrawList->AddRectFilled(
        ImVec2(SplitterRect.Min.X, SplitterRect.Min.Y),
        ImVec2(SplitterRect.Max.X, SplitterRect.Max.Y),
        SplitterColor
    );
}

void SSplitter::OnUpdate(float DeltaSeconds)
{

    UpdateChildRects();

    if (SideLT) SideLT->OnUpdate(DeltaSeconds);
    if (SideRB) SideRB->OnUpdate(DeltaSeconds);
}

void SSplitter::OnMouseMove(FVector2D MousePos)
{
    if (bIsDragging)
    {
        UpdateDrag(MousePos);
    }

    // 마우스 커서 변경
    if (IsMouseOnSplitter(MousePos))
    {
        ImGui::SetMouseCursor(GetMouseCursor());
    }

    // 자식 윈도우에 이벤트 전달
    if (SideLT && SideLT->IsHover(MousePos)) SideLT->OnMouseMove(MousePos);
    if (SideRB && SideRB->IsHover(MousePos)) SideRB->OnMouseMove(MousePos);
}

void SSplitter::OnMouseDown(FVector2D MousePos , uint32 Button)
{
    StartDrag(MousePos);

    // 자식 윈도우에 이벤트 전달
    if (SideLT && SideLT->IsHover(MousePos)) SideLT->OnMouseDown(MousePos, Button);
    if (SideRB && SideRB->IsHover(MousePos)) SideRB->OnMouseDown(MousePos, Button);
}


void SSplitter::OnMouseUp(FVector2D MousePos, uint32 Button)
{
    EndDrag();

    // 자식 윈도우에 이벤트 전달
    if (SideLT && SideLT->IsHover(MousePos)) SideLT->OnMouseUp(MousePos, Button);
    if (SideRB && SideRB->IsHover(MousePos)) SideRB->OnMouseUp(MousePos, Button);
}

void SSplitter::SaveToConfig(const FString& SectionName) const
{
    // editor.ini 파일에 스플리터 설정 저장
    std::string filename = "editor.ini";
    std::string searchKey = std::string(SectionName.c_str()) + "_SplitRatio";

    // 기존 파일 읽기
    std::vector<std::string> lines;
    std::ifstream inFile(filename);
    bool keyFound = false;

    if (inFile.is_open())
    {
        std::string line;
        while (std::getline(inFile, line))
        {
            size_t pos = line.find(" = ");
            if (pos != std::string::npos)
            {
                std::string key = line.substr(0, pos);
                if (key == searchKey)
                {
                    line = searchKey + " = " + std::to_string(SplitRatio);
                    keyFound = true;
                }
            }
            lines.push_back(line);
        }
        inFile.close();
    }

    // 키가 없으면 추가
    if (!keyFound)
    {
        lines.push_back(searchKey + " = " + std::to_string(SplitRatio));
    }

    // 파일 다시 쓰기
    std::ofstream outFile(filename);
    if (outFile.is_open())
    {
        for (const auto& line : lines)
        {
            outFile << line << std::endl;
        }
        outFile.close();
    }
}

void SSplitter::LoadFromConfig(const FString& SectionName)
{
    // editor.ini 파일에서 스플리터 설정 로드
    std::ifstream file("editor.ini");
    if (file.is_open())
    {
        std::string line;
        std::string searchKey = std::string(SectionName.c_str()) + "_SplitRatio";

        while (std::getline(file, line))
        {
            size_t pos = line.find(" = ");
            if (pos != std::string::npos)
            {
                std::string key = line.substr(0, pos);
                if (key == searchKey)
                {
                    std::string value = line.substr(pos + 3);
                    SplitRatio = std::stof(value);
                    SplitRatio = FMath::Clamp(SplitRatio, 0.01f, 0.99f);
                    break;
                }
            }
        }
        file.close();
    }
}


