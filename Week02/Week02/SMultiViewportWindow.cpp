#include "pch.h"
#include "SMultiViewportWindow.h"
#include "SWindow.h"
#include "SSplitterV.h"
#include "ImGui/imgui.h"
#include"SceneIOWindow.h"
extern float CLIENTWIDTH;
extern float CLIENTHEIGHT;

void SaveSplitterConfig(SSplitter* RootSplitter)
{
	if (!RootSplitter) return;

	EditorINI["RootSplitter"] = std::to_string(RootSplitter->SplitRatio);

	if (auto TopPanel = dynamic_cast<SSplitterV*>(RootSplitter->SideLT)) {
		EditorINI["TopPanel"] = std::to_string(TopPanel->SplitRatio);

		{
			if (auto LeftPanel = dynamic_cast<SSplitterV*>(RootSplitter->SideLT ? ((SSplitterV*)RootSplitter->SideLT)->SideLT : nullptr))
			{
				EditorINI["LeftPanel"] = std::to_string(LeftPanel->SplitRatio);

				if (auto LeftTop = dynamic_cast<SSplitterH*>(LeftPanel->SideLT))
					EditorINI["LeftTop"] = std::to_string(LeftTop->SplitRatio);

				if (auto LeftBottom = dynamic_cast<SSplitterH*>(LeftPanel->SideRB))
					EditorINI["LeftBottom"] = std::to_string(LeftBottom->SplitRatio);
			}
		}

		if (auto BottomPanel = dynamic_cast<SSplitterV*>(RootSplitter->SideRB))
			EditorINI["BottomPanel"] = std::to_string(BottomPanel->SplitRatio);
	}
}

// ------------------------------------------------------------
// Splitter 비율 불러오기
// ------------------------------------------------------------
void LoadSplitterConfig(SSplitter* RootSplitter)
{
	if (!RootSplitter) return;

	if (EditorINI.Contains("RootSplitter"))
		RootSplitter->SplitRatio = std::stof(EditorINI["RootSplitter"]);

	{
		if (auto TopPanel = dynamic_cast<SSplitterV*>(RootSplitter->SideLT))
		{
			if (EditorINI.Contains("TopPanel"))
				TopPanel->SplitRatio = std::stof(EditorINI["TopPanel"]);

			{

				if (auto LeftPanel = dynamic_cast<SSplitterV*>(TopPanel->SideLT))
				{
					if (EditorINI.Contains("LeftPanel"))
						LeftPanel->SplitRatio = std::stof(EditorINI["LeftPanel"]);

					if (auto LeftTop = dynamic_cast<SSplitterH*>(LeftPanel->SideLT))
						if (EditorINI.Contains("LeftTop"))
							LeftTop->SplitRatio = std::stof(EditorINI["LeftTop"]);

					if (auto LeftBottom = dynamic_cast<SSplitterH*>(LeftPanel->SideRB))
						if (EditorINI.Contains("LeftBottom"))
							LeftBottom->SplitRatio = std::stof(EditorINI["LeftBottom"]);
				}

			}
		}


		if (auto BottomPanel = dynamic_cast<SSplitterV*>(RootSplitter->SideRB))
			if (EditorINI.Contains("BottomPanel"))
				BottomPanel->SplitRatio = std::stof(EditorINI["BottomPanel"]);
	}
}

SMultiViewportWindow::SMultiViewportWindow()
{
	for (int i = 0; i < 4; i++)
		Viewports[i] = nullptr;
}

SMultiViewportWindow::~SMultiViewportWindow()
{
	OnShutdown();
	delete RootSplitter;
	for (int i = 0; i < 4; i++)
		delete Viewports[i];


}

void SMultiViewportWindow::Initialize(ID3D11Device* Device, UWorld* World, const FRect& InRect)
{


	Rect = InRect;

	// 최상위: 수평 스플리터 (위: 뷰포트+오른쪽UI, 아래: Console/Property)
	RootSplitter = new SSplitterH();
	//RootSplitter->SetSplitRatio(0.8);

	
	RootSplitter->SetRect(Rect.Min.X, Rect.Min.Y, Rect.Max.X, Rect.Max.Y);

	// === 위쪽: 좌(4뷰포트) + 우(SceneIO) ===

	SSplitterV* TopPanel = new SSplitterV();
	TopPanel->SetSplitRatio(0.7f);


	// 왼쪽: 4분할 뷰포트
	SSplitterV* LeftPanel = new SSplitterV();
	SSplitterH*	LeftTop = new SSplitterH();
	SSplitterH* LeftBottom = new SSplitterH();
	LeftPanel->SideLT = LeftTop;
	LeftPanel->SideRB = LeftBottom;

	// 오른쪽: SceneIO UI
	SceneIOPanel = new SSceneIOWindow();
	SceneIOPanel->SetRect(Rect.Max.X - 300, Rect.Min.Y, Rect.Max.X, Rect.Max.Y);

	// TopPanel 좌우 배치
	TopPanel->SideLT = LeftPanel;
	TopPanel->SideRB = SceneIOPanel;

	// === 아래쪽: Console + Property ===
	SSplitterV* BottomPanel = new SSplitterV();
	//BottomPanel->SetSplitRatio(0.7);
	ConsolePanel = new SSceneIOWindow();   // 직접 만든 ConsoleWindow 클래스
	PropertyPanel = new SSceneIOWindow();  // 직접 만든 PropertyWindow 클래스
	BottomPanel->SideLT = ConsolePanel;
	BottomPanel->SideRB = PropertyPanel;

	// 최상위 스플리터에 연결
	RootSplitter->SideLT = TopPanel;
	RootSplitter->SideRB = BottomPanel;

	// === 뷰포트 생성 ===
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
		Rect.GetWidth() / 2, Rect.GetHeight(),
		World, Device, EViewportType::Orthographic_Left);

	Viewports[3]->Initialize(Rect.GetWidth() / 2, Rect.GetHeight() / 2,
		Rect.GetWidth(), Rect.GetHeight(),
		World, Device, EViewportType::Orthographic_Top);

	// 뷰포트들을 2x2로 연결
	LeftTop->SideLT = Viewports[0];
	LeftTop->SideRB = Viewports[1];
	LeftBottom->SideLT = Viewports[2];
	LeftBottom->SideRB = Viewports[3];

	LoadSplitterConfig(RootSplitter);
}

void SMultiViewportWindow::OnRender()
{
	if (RootSplitter)
	{
		RootSplitter->OnRender();

		// 뷰포트 간 경계선을 더 명확하게 표시
	   // ImDrawList* DrawList = ImGui::GetBackgroundDrawList();

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

void SMultiViewportWindow::OnShutdown()
{
	SaveSplitterConfig(RootSplitter);
	//SaveEditorINI("editor.ini");
}