#include "pch.h"
#include "Window/UIWindow.h"
#include "ImGui/ImGuiHelper.h"
#include "Widget/Widget.h"
#include "../ImGui/imgui.h"
#include "../ImGui/imgui_internal.h"
#include "../ImGui/imgui_impl_dx11.h"
#include "../imGui/imgui_impl_win32.h"


UUIManager::UUIManager()
{
	ImGuiHelper = NewObject<UImGuiHelper>();
	Initialize();
}

UUIManager::~UUIManager()
{
}

UUIManager& UUIManager::GetInstance()
{
	static UUIManager* Instance = nullptr;
	if (Instance == nullptr)
	{
		Instance = NewObject<UUIManager>();
	}
	return *Instance;
}

/**
 * @brief UI 매니저 초기화
 */
void UUIManager::Initialize()
{
	if (bIsInitialized)
	{
		return;
	}

	UE_LOG("UIManager: UI System 초기화 진행 중...");

	UIWindows.clear();
	FocusedWindow = nullptr;
	TotalTime = 0.0f;

	bIsInitialized = true;

	UE_LOG("UIManager: UI System 초기화 성공");
}

/**
 * @brief 기존 UIManager 호환 초기화 메서드
 * main.cpp에서 호출되는 인터페이스
 */
void UUIManager::Initialize(HWND hWindow, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext)
{
	// 기본 초기화
	Initialize();
	
	// ImGui 초기화 (device와 context 포함)
	if (ImGuiHelper)
	{
		ImGuiHelper->Initialize(hWindow, InDevice, InDeviceContext);
	}
}

/**
 * @brief UI 매니저 종료 및 정리
 */
void UUIManager::Shutdown()
{
	if (!bIsInitialized)
	{
		return;
	}

	UE_LOG("UIManager: Shutting Down UI system...");

	// ImGui 정리
	if (ImGuiHelper)
	{
		DeleteObject(ImGuiHelper);
	}

	// 모든 UI 윈도우 정리
	for (auto* Window : UIWindows)
	{
		if (Window && !Window->IsSingleton())
		{
			Window->Cleanup();
			delete Window;
		}
	}

	UIWindows.clear();
	FocusedWindow = nullptr;
	bIsInitialized = false;

	UE_LOG("UIManager: UI System Shut Down Successfully.");
}

/**
 * @brief 모든 UI 윈도우 업데이트
 */
void UUIManager::Update()
{
	Update(0.0f); // 기본 Update는 DeltaTime 0으로 호출
}

/**
 * @brief 모든 UI 윈도우 업데이트 (DeltaTime 포함)
 */
void UUIManager::Update(float DeltaTime)
{
	if (!bIsInitialized)
	{
		return;
	}

	// DeltaTime 저장 및 총 시간 누적
	CurrentDeltaTime = DeltaTime;
	TotalTime += DeltaTime;

	// 모든 UI 윈도우 업데이트
	for (auto* Window : UIWindows)
	{
		if (Window && Window->IsVisible())
		{
			Window->Update();
		}
	}

	// 포커스 상태 업데이트
	UpdateFocusState();

	// 키보드 단축키 처리
	HandleViewportShortcuts();
}

/**
 * @brief 모든 UI 윈도우 렌더링
 */
void UUIManager::Render()
{
	if (!bIsInitialized)
	{
		return;
	}

	if (!ImGuiHelper)
	{
		return;
	}

	// ImGui 프레임 시작
	ImGuiHelper->BeginFrame();

	// 우선순위에 따라 정렬 (필요한 경우에만 진행)
	// SortUIWindowsByPriority();

	// 모든 UI 윈도우 렌더링
	for (auto* Window : UIWindows)
	{
		if (Window)
		{
			Window->RenderWindow();
		}
	}


	
}
void UUIManager::EndFrame() {

	// 뷰포트 전환 UI 렌더링
	RenderViewportSwitcher();

	// ImGui 프레임 종료
	ImGuiHelper->EndFrame();
}


/**
 * @brief UI 윈도우 등록
 * @param InWindow 등록할 UI 윈도우
 * @return 등록 성공 여부
 */
bool UUIManager::RegisterUIWindow(UUIWindow* InWindow)
{
	if (!InWindow)
	{
		UE_LOG("UIManager: Error: Attempted To Register Null Window!");
		return false;
	}

	// 이미 등록된 윈도우인지 확인
	auto Iter = std::find(UIWindows.begin(), UIWindows.end(), InWindow);
	if (Iter != UIWindows.end())
	{
		UE_LOG("UIManager: Warning: Window Already Registered: %d", InWindow->GetWindowID());
		return false;
	}

	// 윈도우 초기화
	try
	{
		InWindow->Initialize();
	}
	catch (const std::exception& e)
	{
		UE_LOG("UIManager: Error: Failed To Initialize Window %d: %s", InWindow->GetWindowID(), e.what());
		return false;
	}

	UIWindows.push_back(InWindow);

	UE_LOG("UIManager: Registered UI Window: %d (%s)", InWindow->GetWindowID(), InWindow->GetWindowTitle().c_str());
	UE_LOG("UIManager: Total Registered Windows: %zu", UIWindows.size());

	return true;
}

/**
 * @brief UI 윈도우 등록 해제
 * @param InWindow 해제할 UI 윈도우
 * @return 해제 성공 여부
 */
bool UUIManager::UnregisterUIWindow(UUIWindow* InWindow)
{
	if (!InWindow)
	{
		return false;
	}

	auto It = std::find(UIWindows.begin(), UIWindows.end(), InWindow);
	if (It == UIWindows.end())
	{
		UE_LOG("UIManager: Warning: Attempted to unregister non-existent window: %d", InWindow->GetWindowID());
		return false;
	}

	// 포커스된 윈도우였다면 포커스 해제
	if (FocusedWindow == InWindow)
	{
		FocusedWindow = nullptr;
	}

	// 윈도우 정리
	InWindow->Cleanup();

	UIWindows.erase(It);

	UE_LOG("UIManager: Unregistered UI Window: %d", InWindow->GetWindowID());
	UE_LOG("UIManager: Total Registered Windows: %zu", UIWindows.size());

	return true;
}

/**
 * @brief 이름으로 UI 윈도우 검색
 * @param InWindowName 검색할 윈도우 제목
 * @return 찾은 윈도우 (없으면 nullptr)
 */
UUIWindow* UUIManager::FindUIWindow(const FString& InWindowName) const
{
	for (auto* Window : UIWindows)
	{
		if (Window && Window->GetWindowTitle() == InWindowName)
		{
			return Window;
		}
	}
	return nullptr;
}

UWidget* UUIManager::FindWidget(const FString& InWidgetName) const
{
	for (auto* Window : UIWindows)
	{
		for (auto* Widget : Window->GetWidgets())
		{
			if (Widget->GetName() == InWidgetName)
			{
				return Widget;
			}
		}
	}
	return nullptr;
}

/**
 * @brief 모든 UI 윈도우 숨기기
 */
void UUIManager::HideAllWindows() const
{
	for (auto* Window : UIWindows)
	{
		if (Window)
		{
			Window->SetWindowState(EUIWindowState::Hidden);
		}
	}
	UE_LOG("UIManager: All Windows Hidden.");
}

/**
 * @brief 모든 UI 윈도우 보이기
 */
void UUIManager::ShowAllWindows() const
{
	for (auto* Window : UIWindows)
	{
		if (Window)
		{
			Window->SetWindowState(EUIWindowState::Visible);
		}
	}
	UE_LOG("UIManager: All Windows Shown.");
}

/**
 * @brief 특정 윈도우에 포커스 설정
 */
void UUIManager::SetFocusedWindow(UUIWindow* InWindow)
{
	if (FocusedWindow != InWindow)
	{
		if (FocusedWindow)
		{
			FocusedWindow->OnFocusLost();
		}

		FocusedWindow = InWindow;

		if (FocusedWindow)
		{
			FocusedWindow->OnFocusGained();
		}
	}
}

/**
 * @brief 디버그 정보 출력
 * 필요한 지점에서 호출해서 로그로 체크하는 용도
 */
void UUIManager::PrintDebugInfo() const
{
	UE_LOG("");
	UE_LOG("=== UI Manager Debug Info ===");
	UE_LOG("Initialized: %s", (bIsInitialized ? "Yes" : "No"));
	UE_LOG("Total Time: %.2fs", TotalTime);
	UE_LOG("Registered Windows: %zu", UIWindows.size());
	//UE_LOG("Focused Window: %s", (FocusedWindow ? to_string(FocusedWindow->GetWindowID()).c_str() : "None"));

	UE_LOG("");
	UE_LOG("--- Window List ---");
	for (size_t i = 0; i < UIWindows.size(); ++i)
	{
		auto* Window = UIWindows[i];
		if (Window)
		{
			UE_LOG("[%zu] %d (%s)", i, Window->GetWindowID(), Window->GetWindowTitle().c_str());
			UE_LOG("    State: %s", (Window->IsVisible() ? "Visible" : "Hidden"));
			UE_LOG("    Priority: %d", Window->GetPriority());
			UE_LOG("    Focused: %s", (Window->IsFocused() ? "Yes" : "No"));
		}
	}
	UE_LOG("===========================");
	UE_LOG("");
}

/**
 * @brief UI 윈도우들을 우선순위에 따라 정렬
 */
void UUIManager::SortUIWindowsByPriority()
{
	// 우선순위가 낮을수록 먼저 렌더링되고 가려짐
	std::sort(UIWindows.begin(), UIWindows.end(), [](const UUIWindow* A, const UUIWindow* B)
	{
		if (!A) return false;
		if (!B) return true;
		return A->GetPriority() < B->GetPriority();
	});
}

/**
 * @brief 포커스 상태 업데이트
 */
void UUIManager::UpdateFocusState()
{
	// ImGui에서 현재 포커스된 윈도우 찾기
	UUIWindow* NewFocusedWindow = nullptr;

	for (auto* Window : UIWindows)
	{
		if (Window && Window->IsVisible() && Window->IsFocused())
		{
			NewFocusedWindow = Window;
			break;
		}
	}

	// 포커스 변경시 처리
	if (FocusedWindow != NewFocusedWindow)
	{
		SetFocusedWindow(NewFocusedWindow);
	}
}

/**
 * @brief 윈도우 프로시저 핸들러
 */
LRESULT UUIManager::WndProcHandler(HWND hwnd, uint32 msg, WPARAM wParam, LPARAM lParam)
{
	return UImGuiHelper::WndProcHandler(hwnd, msg, wParam, lParam);
}

void UUIManager::RepositionImGuiWindows()
{
	// 1. 현재 화면(Viewport)의 작업 영역을 가져옵니다.
	for (auto& window : UIWindows)
	{
		window->SetIsResized(true);
	}
}

/**
 * @brief 마우스 회전 값 업데이트
 * 카메라 제어를 위한 회전 상태 관리
 */
void UUIManager::UpdateMouseRotation(float InPitch, float InYaw)
{
	TempCameraRotation.X = InPitch;
	TempCameraRotation.Y = InYaw;
	// Z(Roll)은 별도 관리
}

/**
 * @brief 월드의 선택된 액터 반환
 * Widget에서 선택된 액터에 접근할 수 있도록 도움
 */
AActor* UUIManager::GetSelectedActor() const
{
	// PickedActorRef의 유효성 확인
	if (PickedActorRef)
	{
		try
		{
			// 액터가 여전히 유효한지 신벃할 수 있는 간단한 테스트
			// 액터의 기본 메서드를 호출해보자
			if (PickedActorRef->GetClass() != nullptr)
			{
				return PickedActorRef;
			}
		}
		catch (...)
		{
			// 삭제된 액터에 접근한 경우 참조 정리
			const_cast<UUIManager*>(this)->PickedActorRef = nullptr;
		}
	}
	return nullptr;
}

/**
 * @brief 액터 선택 설정 (안전하게)
 */
void UUIManager::SetPickedActor(AActor* InPickedActor)
{
	if (InPickedActor)
	{
		try
		{
			// 기존 선택 해제
			ResetPickedActor();
			
			// 새 액터 선택
			PickedActorRef = InPickedActor;
			PickedActorRef->SetIsPicked(true);
		}
		catch (...)
		{
			// 유효하지 않은 액터인 경우
			PickedActorRef = nullptr;
		}
	}
}

/**
 * @brief 액터 선택 해제 (안전하게)
 */
void UUIManager::ResetPickedActor()
{
	if (PickedActorRef)
	{
		try
		{
			PickedActorRef->SetIsPicked(false);
		}
		catch (...)
		{
			// 삭제된 액터인 경우 예외 무시
		}
		PickedActorRef = nullptr;
	}
}

/**
 * @brief 뷰포트 전환 UI 렌더링
 */
void UUIManager::RenderViewportSwitcher()
{
	// 화면 상단 중앙에 고정된 위치에 버튼 표시
	ImGuiViewport* viewport = ImGui::GetMainViewport();
	ImVec2 work_pos = viewport->WorkPos;
	ImVec2 work_size = viewport->WorkSize;

	// 버튼 크기 설정
	float button_width = 120.0f;
	float button_height = 35.0f;
	float spacing = 15.0f;

	// 중앙 위치 계산
	float center_x = work_pos.x + (work_size.x - (button_width * 2 + spacing)) * 0.5f;
	float top_y = work_pos.y + 15.0f;

	// 윈도우 플래그 설정 (타이틀바 없음, 크기 조정 불가, 이동 불가)
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoMove
		| ImGuiWindowFlags_NoCollapse
		| ImGuiWindowFlags_AlwaysAutoResize
		| ImGuiWindowFlags_NoBackground
		| ImGuiWindowFlags_NoScrollbar;

	// 윈도우 위치 설정
	ImGui::SetNextWindowPos(ImVec2(center_x, top_y), ImGuiCond_Always);

	if (ImGui::Begin("ViewportSwitcher", nullptr, window_flags))
	{
		// 현재 상태 표시
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.8f, 0.8f, 0.8f, 1.0f));
		ImGui::Text("Viewport Mode:");
		ImGui::PopStyleColor();

		ImGui::SameLine();
		ImGui::Spacing();
		ImGui::SameLine();

		// 메인 뷰포트 버튼
		if (bUseMainViewport)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		}

		if (ImGui::Button("Single View", ImVec2(button_width, button_height)))
		{
			SetViewportMode(true);
			UE_LOG("UIManager: Switched to Main Viewport mode");
		}

		// 툴팁 추가
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Switch to single perspective viewport\nShortcut: F1");
		}

		ImGui::PopStyleColor(3);

		ImGui::SameLine();

		// 멀티 뷰포트 버튼
		if (!bUseMainViewport)
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.6f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.7f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.1f, 0.5f, 0.1f, 1.0f));
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
		}

		if (ImGui::Button("Multi View", ImVec2(button_width, button_height)))
		{
			SetViewportMode(false);
			UE_LOG("UIManager: Switched to Multi Viewport mode");
		}

		// 툴팁 추가
		if (ImGui::IsItemHovered())
		{
			ImGui::SetTooltip("Switch to 4-panel viewport (Perspective, Front, Left, Top)\nShortcut: F2");
		}

		ImGui::PopStyleColor(3);

		// 현재 활성 모드 표시
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.8f, 1.0f, 1.0f));
		if (bUseMainViewport)
		{
			ImGui::Text("Active: Single Perspective View");
		}
		else
		{
			ImGui::Text("Active: 4-Panel Multi View");
		}
		ImGui::PopStyleColor();

		// 단축키 정보 표시
		ImGui::Spacing();
		ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
		ImGui::Text("Shortcuts: F1/F2 or Ctrl+V to toggle");
		ImGui::PopStyleColor();
	}
	ImGui::End();
}

/**
 * @brief 뷰포트 전환을 위한 키보드 단축키 처리
 */
void UUIManager::HandleViewportShortcuts()
{
	// Ctrl + V: 뷰포트 모드 전환
	if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl) && ImGui::IsKeyPressed(ImGuiKey_V))
	{
		ToggleViewportMode();
		UE_LOG("UIManager: Viewport mode toggled via keyboard shortcut (Ctrl+V)");
	}

	// F1: 단일 뷰포트 모드
	if (ImGui::IsKeyPressed(ImGuiKey_F1))
	{
		SetViewportMode(true);
		UE_LOG("UIManager: Switched to Single Viewport mode via F1");
	}

	// F2: 멀티 뷰포트 모드
	if (ImGui::IsKeyPressed(ImGuiKey_F2))
	{
		SetViewportMode(false);
		UE_LOG("UIManager: Switched to Multi Viewport mode via F2");
	}
}

/**
 * @brief 뷰포트 모드 설정 (개선된 버전)
 */
void UUIManager::SetViewportMode(bool bUseMain)
{
	if (bUseMainViewport != bUseMain)
	{
		bool bPreviousMode = bUseMainViewport;
		bUseMainViewport = bUseMain;

		// 상세한 로깅
		if (bUseMain)
		{
			UE_LOG("UIManager: Viewport mode changed to Single View (Main Viewport)");
			UE_LOG("UIManager: Now using single perspective viewport for detailed work");
		}
		else
		{
			UE_LOG("UIManager: Viewport mode changed to Multi View (4-Panel Layout)");
			UE_LOG("UIManager: Now using 4-panel view: Perspective, Front, Left, Top");
		}

		// UI 윈도우들의 위치 재조정이 필요할 수 있음
		RepositionImGuiWindows();
	}
	else
	{
		// 이미 같은 모드인 경우
		UE_LOG("UIManager: Viewport mode already set to %s",
			   bUseMain ? "Single View" : "Multi View");
	}
}

/**
 * @brief 뷰포트 모드 토글 (개선된 버전)
 */
void UUIManager::ToggleViewportMode()
{
	SetViewportMode(!bUseMainViewport);
	UE_LOG("UIManager: Viewport mode toggled");
}
