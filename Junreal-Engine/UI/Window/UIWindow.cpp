﻿#include "pch.h"
#include "UIWindow.h"
#include "../../ImGui/imgui_internal.h"
#include "../Widget/Widget.h"

using namespace std;

int UUIWindow::IssuedWindowID = 0;

UUIWindow::UUIWindow(const FUIWindowConfig& InConfig)
	: Config(InConfig)
	  , CurrentState(InConfig.InitialState)
	  , bIsWindowOpen(true)  // ImGui 윈도우가 열린 상태로 초기화
{
	// 고유한 윈도우 ID 생성
	WindowID = ++IssuedWindowID;

	// 윈도우 플래그 업데이트
	Config.UpdateWindowFlags();

	// 초기 상태 설정
	LastWindowSize = Config.DefaultSize;
	LastWindowPosition = Config.DefaultPosition;

	if (IssuedWindowID == 1)
	{
		cout << "UIWindow: Created: " << WindowID << " (" << Config.WindowTitle << ")" << "\n";
	}
	else
	{
		UE_LOG("UIWindow: Created: %d (%s)", WindowID, Config.WindowTitle.c_str());
	}
}

UUIWindow::~UUIWindow()
{
	for (UWidget* Widget : Widgets)
	{
		DeleteObject(Widget);
		Widget = nullptr;
	}
}

void UUIWindow::Initialize()
{
}

/**
 * @brief 뷰포트가 리사이징 되었을 때 앵커/좌상단 기준 상대 위치 비율을 고정하는 로직
 */
void UUIWindow::OnMainWindowResized()
{
	if (!ImGui::GetCurrentContext() || !IsVisible())
		return;

	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	const ImVec2 currentViewportSize = viewport->WorkSize;

	if (currentViewportSize.x <= 0 || currentViewportSize.y <= 0)
		return;

 	const ImVec2 anchor = PositionRatio;
	const ImVec2 pivot = { 0.f, 0.f };

	ImVec2 responsiveSize(
		currentViewportSize.x * SizeRatio.x,
		currentViewportSize.y * SizeRatio.y
	);

	ImVec2 targetPos(
		viewport->WorkPos.x + currentViewportSize.x * anchor.x,
		viewport->WorkPos.y + currentViewportSize.y * anchor.y
	);

	ImVec2 finalPos(
		targetPos.x - responsiveSize.x * pivot.x,
		targetPos.y - responsiveSize.y * pivot.y
	);

	ImGui::SetWindowSize(responsiveSize, ImGuiCond_Always);
	ImGui::SetWindowPos(finalPos, ImGuiCond_Always);
}

/**
 * @brief 윈도우가 뷰포트 범위 밖으로 나갔을 시 클램프 하는 로직
 */
void UUIWindow::ClampWindow()
{
	if (!IsVisible())
	{
		return;
	}

	const ImGuiViewport* Viewport = ImGui::GetMainViewport();
	const ImVec2 WorkPos = Viewport->WorkPos;
	const ImVec2 WorkSize = Viewport->WorkSize;

	ImVec2 pos = LastWindowPosition;
	ImVec2 size = LastWindowSize;

	bool size_changed = false;
	if (size.x > WorkSize.x) { size.x = WorkSize.x; size_changed = true; }
	if (size.y > WorkSize.y) { size.y = WorkSize.y; size_changed = true; }
	if (size_changed)
	{
		ImGui::SetWindowSize(size);
	}
	bool pos_changed = false;
	if (pos.x + size.x > WorkPos.x + WorkSize.x) { pos.x = WorkPos.x + WorkSize.x - size.x; pos_changed = true; }
	if (pos.y + size.y > WorkPos.y + WorkSize.y) { pos.y = WorkPos.y + WorkSize.y - size.y; pos_changed = true; }
	if (pos.x < WorkPos.x) { pos.x = WorkPos.x; pos_changed = true; }
	if (pos.y < WorkPos.y) { pos.y = WorkPos.y; pos_changed = true; }
	if (pos_changed)
	{
		ImGui::SetWindowPos(pos);
	}
}

/**
 * @brief 내부적으로 사용되는 윈도우 렌더링 처리
 * 서브클래스에서 직접 호출할 수 없도록 접근한정자 설정
 */
void UUIWindow::RenderWindow()
{
	// 숨겨진 상태면 렌더링하지 않음
	if (!IsVisible())
	{
		return;
	}

	// 도킹 설정 적용
	ApplyDockingSettings();

	// ImGui 윈도우 시작
	ImGui::SetNextWindowSize(Config.DefaultSize, ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowPos(Config.DefaultPosition, ImGuiCond_FirstUseEver);

	// 크기 제한 설정
	//ImGui::SetNextWindowSizeConstraints(Config.MinSize, Config.MaxSize);

	bool bIsOpen = bIsWindowOpen;

	if (ImGui::Begin(Config.WindowTitle.c_str(), &bIsOpen, Config.WindowFlags))
	{
		if (!bIsResized)
		{
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 currentPos = ImGui::GetWindowPos();
			ImVec2 currentSize = ImGui::GetWindowSize();
			ImVec2 pivot = { 0.f, 0.f };

			PositionRatio.x = (currentPos.x - viewport->WorkPos.x + currentSize.x * pivot.x) / viewport->WorkSize.x;
			PositionRatio.y = (currentPos.y - viewport->WorkPos.y + currentSize.y * pivot.y) / viewport->WorkSize.y;

			SizeRatio.x = currentSize.x  / viewport->WorkSize.x;
			SizeRatio.y = currentSize.y  / viewport->WorkSize.y;
		}
		// 실제 UI 컨텐츠 렌더링
		RenderWidget();

		// 윈도우 정보 업데이트
		UpdateWindowInfo();
	}
	//ClampWindow();
	if (bIsResized)
	{
		OnMainWindowResized();
		bIsResized = false;
	}

	ImGui::End();

	// 윈도우가 닫혔는지 확인
	if (!bIsWindowOpen && bIsWindowOpen)
	{
		if (OnWindowClose())
		{
			bIsWindowOpen = false;
			SetWindowState(EUIWindowState::Hidden);
		}
		else
		{
			// 닫기 취소
			bIsWindowOpen = true;
		}
	}
}

void UUIWindow::RenderWidget() const
{
	for (auto* Widget : Widgets)
	{
		if (Widget != nullptr)  // Null check to prevent crashes
		{
			Widget->RenderWidget();
			Widget->PostProcess(); // 선택
		}
	}
}

void UUIWindow::Update() const
{
	for (size_t i = 0; i < Widgets.size(); ++i)
	{
		UWidget* Widget = Widgets[i];
		if (Widget != nullptr)  // Null check to prevent crashes
		{
			try
			{
				Widget->Update();
			}
			catch (...)
			{
				UE_LOG("UIWindow::Update() - Exception in widget %zu (%s)", i, 
				       Widget->GetName().empty() ? "Unknown" : Widget->GetName().c_str());
			}
		}
		else
		{
			UE_LOG("UIWindow::Update() - Null widget at index %zu", i);
		}
	}
}

/**
 * @brief 윈도우 위치와 크기 자동 설정
 */
void UUIWindow::ApplyDockingSettings() const
{
	ImGuiIO& IO = ImGui::GetIO();
	float ScreenWidth = IO.DisplaySize.x;
	float ScreenHeight = IO.DisplaySize.y;

	switch (Config.DockDirection)
	{
	case EUIDockDirection::Left:
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Config.DefaultSize.x, ScreenHeight), ImGuiCond_Always);
		break;

	case EUIDockDirection::Right:
		ImGui::SetNextWindowPos(ImVec2(ScreenWidth - Config.DefaultSize.x, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(Config.DefaultSize.x, ScreenHeight), ImGuiCond_Always);
		break;

	case EUIDockDirection::Top:
		ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(ScreenWidth, Config.DefaultSize.y), ImGuiCond_Always);
		break;

	case EUIDockDirection::Bottom:
		ImGui::SetNextWindowPos(ImVec2(0, ScreenHeight - Config.DefaultSize.y), ImGuiCond_Always);
		ImGui::SetNextWindowSize(ImVec2(ScreenWidth, Config.DefaultSize.y), ImGuiCond_Always);
		break;

	case EUIDockDirection::Center:
		{
			ImVec2 Center = ImVec2(ScreenWidth * 0.5f, ScreenHeight * 0.5f);
			ImVec2 WindowPosition = ImVec2(Center.x - Config.DefaultSize.x * 0.5f,
			                               Center.y - Config.DefaultSize.y * 0.5f);
			ImGui::SetNextWindowPos(WindowPosition, ImGuiCond_Always);
		}
		break;

	case EUIDockDirection::None:
	default:
		// 기본 위치 사용
		break;
	}
}

/**
 * @brief ImGui 컨텍스트에서 현재 윈도우 정보 업데이트
 */
void UUIWindow::UpdateWindowInfo()
{
	// 현재 윈도우가 포커스되어 있는지 확인
	bool bCurrentlyFocused = ImGui::IsWindowFocused();

	if (bCurrentlyFocused != bIsFocused)
	{
		bIsFocused = bCurrentlyFocused;

		if (bIsFocused)
		{
			OnFocusGained();
		}
		else
		{
			OnFocusLost();
		}
	}

	// 윈도우 크기와 위치 업데이트
	LastWindowSize = ImGui::GetWindowSize();
	LastWindowPosition = ImGui::GetWindowPos();
}
