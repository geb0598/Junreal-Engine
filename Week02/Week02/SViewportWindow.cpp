#include "pch.h"
#include "SViewportWindow.h"
#include "World.h"
#include "ImGui/imgui.h"
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
	if (!Viewport->Initialize(StartX, StartY, Width, Height, Device))
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
	RenderToolbar();
	if (ViewportClient)
		ViewportClient->Draw(Viewport);
	// 툴바 렌더링
	
	Viewport->EndRenderFrame();


}

void SViewportWindow::OnUpdate()
{
	if (!Viewport)
		return;

	if (!Viewport) return;

	// 툴바 높이만큼 뷰포트 영역 조정
	float toolbarHeight = 30.0f;
	uint32 NewStartX = static_cast<uint32>(Rect.Left);
	uint32 NewStartY = static_cast<uint32>(Rect.Top + toolbarHeight);
	uint32 NewWidth = static_cast<uint32>(Rect.Right - Rect.Left);
	uint32 NewHeight = static_cast<uint32>(Rect.Bottom - Rect.Top - toolbarHeight);

	Viewport->Resize(NewStartX, NewStartY, NewWidth, NewHeight);
}

void SViewportWindow::OnMouseMove(FVector2D MousePos)
{
	if (!Viewport) return;

	// 툴바 영역 아래에서만 마우스 이벤트 처리
	float toolbarHeight = 30.0f;
	FVector2D LocalPos = MousePos - FVector2D(Rect.Left, Rect.Top + toolbarHeight);
	if (MousePos.Y > Rect.Top + toolbarHeight)
	{
		Viewport->ProcessMouseMove((int32)LocalPos.X, (int32)LocalPos.Y);
	}
}

void SViewportWindow::OnMouseDown(FVector2D MousePos)
{
	if (!Viewport) return;

	// 툴바 영역 아래에서만 마우스 이벤트 처리
	float toolbarHeight = 30.0f;
	if (MousePos.Y > Rect.Top + toolbarHeight)
	{
		bIsMouseDown = true;
		FVector2D LocalPos = MousePos - FVector2D(Rect.Left, Rect.Top + toolbarHeight);
		Viewport->ProcessMouseButtonDown((int32)LocalPos.X, (int32)LocalPos.Y, 0);
	}
}

void SViewportWindow::OnMouseUp(FVector2D MousePos)
{
	if (!Viewport) return;

	// 툴바 영역 아래에서만 마우스 이벤트 처리
	float toolbarHeight = 30.0f;
	if (MousePos.Y > Rect.Top + toolbarHeight)
	{
		bIsMouseDown = false;
		FVector2D LocalPos = MousePos - FVector2D(Rect.Left, Rect.Top + toolbarHeight);
		Viewport->ProcessMouseButtonUp((int32)LocalPos.X, (int32)LocalPos.Y, 0);
	}
}

void SViewportWindow::RenderToolbar()
{
	if (!Viewport) return;

	// 툴바 영역 크기
	float toolbarHeight = 30.0f;
	ImVec2 toolbarPos(Rect.Left, Rect.Top);
	ImVec2 toolbarSize(Rect.Right - Rect.Left, toolbarHeight);

	// 툴바 위치 지정
	ImGui::SetNextWindowPos(toolbarPos);
	ImGui::SetNextWindowSize(toolbarSize);

	// 뷰포트별 고유한 윈도우 ID
	char windowId[64];
	sprintf_s(windowId, "ViewportToolbar_%p", this);

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus;

	if (ImGui::Begin(windowId, nullptr, flags))
	{
		// 뷰포트 모드 선택 콤보박스
		const char* viewportModes[] = {
			"Perspective",
			"Top",
			"Bottom",
			"Front",
			"Left",
			"Right",
			"Back"
		};

		int currentMode = static_cast<int>(ViewportType);
		ImGui::SetNextItemWidth(100);
		if (ImGui::Combo("##ViewportMode", &currentMode, viewportModes, IM_ARRAYSIZE(viewportModes)))
		{
			EViewportType newType = static_cast<EViewportType>(currentMode);
			if (newType != ViewportType)
			{
				ViewportType = newType;

				// ViewportClient 업데이트
				if (ViewportClient)
				{
					ViewportClient->SetViewportType(ViewportType);
					// 직교 뷰의 경우 카메라 설정 업데이트
					if (ViewportType != EViewportType::Perspective)
					{
						ViewportClient->SetupOrthographicCamera();
					}
				}

				// 뷰포트 이름 업데이트
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
			}
		}
		ImGui::SameLine();

		// 뷰포트 이름 표시
		ImGui::Text("%s", ViewportName.ToString().c_str());
		ImGui::SameLine();

		// 버튼들
		if (ImGui::Button("Move")) { /* TODO: 이동 모드 전환 */ }
		ImGui::SameLine();

		if (ImGui::Button("Rotate")) { /* TODO: 회전 모드 전환 */ }
		ImGui::SameLine();

		if (ImGui::Button("Scale")) { /* TODO: 스케일 모드 전환 */ }
		ImGui::SameLine();

		if (ImGui::Button("Reset")) { /* TODO: 카메라 Reset */ }

		const char* viewModes[] = { "Lit", "Unlit", "Wireframe" };
		int currentViewMode = static_cast<int>(ViewportClient-> GetViewModeIndex())-1; // 0=Lit, 1=Unlit, 2=Wireframe -1이유 1부터 시작이여서 

		ImGui::SameLine();
		if (ImGui::Combo("##ViewMode", &currentViewMode, viewModes, IM_ARRAYSIZE(viewModes)))
		{
			if (ViewportClient)
			{
				switch (currentViewMode)
				{
				case 0: ViewportClient->SetViewModeIndex(EViewModeIndex::VMI_Lit); break;
				case 1: ViewportClient->SetViewModeIndex(EViewModeIndex::VMI_Unlit); break;
				case 2: ViewportClient->SetViewModeIndex(EViewModeIndex::VMI_Wireframe); break;
				}
			}
		}
	}
	ImGui::End();
}