﻿#include "pch.h"
#include <windowsx.h> // GET_X_LPARAM / GET_Y_LPARAM

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#endif
#ifndef GET_Y_LPARAM
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif


UInputManager::UInputManager()
    : WindowHandle(nullptr)
    , MousePosition(0.0f, 0.0f)
    , PreviousMousePosition(0.0f, 0.0f)
    , ScreenSize(1.0f, 1.0f)
    , MouseWheelDelta(0.0f)
{
    // 배열 초기화
    memset(MouseButtons, false, sizeof(MouseButtons));
    memset(PreviousMouseButtons, false, sizeof(PreviousMouseButtons));
    memset(KeyStates, false, sizeof(KeyStates));
    memset(PreviousKeyStates, false, sizeof(PreviousKeyStates));
}

UInputManager::~UInputManager()
{
}

UInputManager& UInputManager::GetInstance()
{
    static UInputManager* Instance = nullptr;
    if (Instance == nullptr)
    {
        Instance = NewObject<UInputManager>();
    }
    return *Instance;
}

void UInputManager::Initialize(HWND hWindow)
{
    WindowHandle = hWindow;
    
    // 현재 마우스 위치 가져오기
    POINT CursorPos;
    GetCursorPos(&CursorPos);
    ScreenToClient(WindowHandle, &CursorPos);
    
    MousePosition.X = static_cast<float>(CursorPos.x);
    MousePosition.Y = static_cast<float>(CursorPos.y);
    PreviousMousePosition = MousePosition;

    // 초기 스크린 사이즈 설정 (클라이언트 영역)
    if (WindowHandle)
    {
        RECT rc{};
        if (GetClientRect(WindowHandle, &rc))
        {
            float w = static_cast<float>(rc.right - rc.left);
            float h = static_cast<float>(rc.bottom - rc.top);
            ScreenSize.X = (w > 0.0f) ? w : 1.0f;
            ScreenSize.Y = (h > 0.0f) ? h : 1.0f;
        }
    }
}

void UInputManager::Update()
{
    // 이전 프레임 상태 저장
    PreviousMousePosition = MousePosition;

    // 마우스 휠 델타 초기화 (프레임마다 리셋)
    MouseWheelDelta = 0.0f;
    
    // 매 프레임마다 실시간 마우스 위치 업데이트
    if (WindowHandle)
    {
        POINT CursorPos;
        if (GetCursorPos(&CursorPos))
        {
            ScreenToClient(WindowHandle, &CursorPos);
            MousePosition.X = static_cast<float>(CursorPos.x);
            MousePosition.Y = static_cast<float>(CursorPos.y);
        }

        // 화면 크기 갱신 (윈도우 리사이즈 대응)
        RECT rc{};
        if (GetClientRect(WindowHandle, &rc))
        {
            float w = static_cast<float>(rc.right - rc.left);
            float h = static_cast<float>(rc.bottom - rc.top);
            ScreenSize.X = (w > 0.0f) ? w : 1.0f;
            ScreenSize.Y = (h > 0.0f) ? h : 1.0f;
        }
    }
    
    // 디버그: 왼쪽 마우스 버튼 상태 변화 확인
    if (bEnableDebugLogging && (MouseButtons[LeftButton] != PreviousMouseButtons[LeftButton]))
    {
        char debugMsg[128];
        sprintf_s(debugMsg, "Update: Mouse state changing from %d to %d\n", 
                  PreviousMouseButtons[LeftButton], MouseButtons[LeftButton]);
        UE_LOG(debugMsg);
    }
    
    memcpy(PreviousMouseButtons, MouseButtons, sizeof(MouseButtons));
    memcpy(PreviousKeyStates, KeyStates, sizeof(KeyStates));
}

void UInputManager::ProcessMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // ImGui 컨텍스트가 존재하는지 확인
    bool imguiWantsMouse = false;
    bool imguiWantsKeyboard = false;
    
    if (ImGui::GetCurrentContext() != nullptr)
    {
        // ImGui가 입력을 사용 중인지 확인
        ImGuiIO& io = ImGui::GetIO();
        imguiWantsMouse = io.WantCaptureMouse;
        imguiWantsKeyboard = io.WantCaptureKeyboard;
        
        // 디버그 출력 (마우스 클릭 시만)
        if (bEnableDebugLogging && message == WM_LBUTTONDOWN)
        {
            char debugMsg[128];
            sprintf_s(debugMsg, "ImGui State - WantMouse: %d, WantKeyboard: %d\n", 
                      imguiWantsMouse, imguiWantsKeyboard);
            UE_LOG(debugMsg);
        }
    }
    else
    {
        // ImGui 컨텍스트가 없으면 게임 입력을 허용
        if (bEnableDebugLogging && message == WM_LBUTTONDOWN)
        {
            UE_LOG("ImGui context not initialized - allowing game input\n");
        }
    }
    
    switch (message)
    {
    case WM_MOUSEMOVE:
        {
            // 항상 마우스 위치는 업데이트 (ImGui 상관없이)
            // 좌표는 16-bit signed. 반드시 GET_X/Y_LPARAM으로 부호 확장해야 함.
            int MouseX = GET_X_LPARAM(lParam);
            int MouseY = GET_Y_LPARAM(lParam);
            UpdateMousePosition(MouseX, MouseY);
        }
        break;
    case WM_SIZE:
        {
            // 클라이언트 영역 크기 업데이트
            int w = LOWORD(lParam);
            int h = HIWORD(lParam);
            if (w <= 0) w = 1;
            if (h <= 0) h = 1;
            ScreenSize.X = static_cast<float>(w);
            ScreenSize.Y = static_cast<float>(h);
        }
        break;
        
    case WM_LBUTTONDOWN:
        if (!imguiWantsMouse)  // ImGui가 마우스를 사용하지 않을 때만
        {
            UpdateMouseButton(LeftButton, true);
            if (bEnableDebugLogging) UE_LOG("InputManager: Left Mouse Down\n");
        }
        break;
        
    case WM_LBUTTONUP:
        if (!imguiWantsMouse)  // ImGui가 마우스를 사용하지 않을 때만
        {
            UpdateMouseButton(LeftButton, false);
            if (bEnableDebugLogging) UE_LOG("InputManager: Left Mouse UP\n");
        }
        break;
        
    case WM_RBUTTONDOWN:
        if (!imguiWantsMouse)
        {
            UpdateMouseButton(RightButton, true);
            if (bEnableDebugLogging) UE_LOG("InputManager: Right Mouse DOWN");
        }
        else
        {
            if (bEnableDebugLogging) UE_LOG("InputManager: Right Mouse DOWN blocked by ImGui");
        }
        break;
        
    case WM_RBUTTONUP:
        if (!imguiWantsMouse)
        {
            UpdateMouseButton(RightButton, false);
            if (bEnableDebugLogging) UE_LOG("InputManager: Right Mouse UP");
        }
        else
        {
            if (bEnableDebugLogging) UE_LOG("InputManager: Right Mouse UP blocked by ImGui");
        }
        break;
        
    case WM_MBUTTONDOWN:
        if (!imguiWantsMouse)
        {
            UpdateMouseButton(MiddleButton, true);
        }
        break;
        
    case WM_MBUTTONUP:
        if (!imguiWantsMouse)
        {
            UpdateMouseButton(MiddleButton, false);
        }
        break;
        
    case WM_XBUTTONDOWN:
        if (!imguiWantsMouse)
        {
            // X버튼 구분 (X1, X2)
            WORD XButton = GET_XBUTTON_WPARAM(wParam);
            if (XButton == XBUTTON1)
                UpdateMouseButton(XButton1, true);
            else if (XButton == XBUTTON2)
                UpdateMouseButton(XButton2, true);
        }
        break;
        
    case WM_XBUTTONUP:
        if (!imguiWantsMouse)
        {
            // X버튼 구분 (X1, X2)
            WORD XButton = GET_XBUTTON_WPARAM(wParam);
            if (XButton == XBUTTON1)
                UpdateMouseButton(XButton1, false);
            else if (XButton == XBUTTON2)
                UpdateMouseButton(XButton2, false);
        }
        break;

    case WM_MOUSEWHEEL:
        if (!imguiWantsMouse)
        {
            // 휠 델타값 추출 (HIWORD에서 signed short로 캐스팅)
            short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            MouseWheelDelta = static_cast<float>(wheelDelta) / WHEEL_DELTA; // 정규화 (-1.0 ~ 1.0)

            if (bEnableDebugLogging)
            {
                char debugMsg[64];
                sprintf_s(debugMsg, "InputManager: Mouse Wheel - Delta: %.2f\n", MouseWheelDelta);
                UE_LOG(debugMsg);
            }
        }
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!imguiWantsKeyboard)  // ImGui가 키보드를 사용하지 않을 때만
        {
            // Virtual Key Code 추출
            int KeyCode = static_cast<int>(wParam);
            UpdateKeyState(KeyCode, true);
            
            // 디버그 출력
            if (bEnableDebugLogging)
            {
                char debugMsg[64];
                sprintf_s(debugMsg, "InputManager: Key Down - %d\n", KeyCode);
                UE_LOG(debugMsg);
            }
        }
        break;
        
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (!imguiWantsKeyboard)  // ImGui가 키보드를 사용하지 않을 때만
        {
            // Virtual Key Code 추출
            int KeyCode = static_cast<int>(wParam);
            UpdateKeyState(KeyCode, false);
            
            // 디버그 출력
            if (bEnableDebugLogging)
            {
                char debugMsg[64];
                sprintf_s(debugMsg, "InputManager: Key UP - %d\n", KeyCode);
                UE_LOG(debugMsg);
            }
        }
        break;
    }

}

bool UInputManager::IsMouseButtonDown(EMouseButton Button) const
{
    if (Button >= MaxMouseButtons) return false;
    return MouseButtons[Button];
}

bool UInputManager::IsMouseButtonPressed(EMouseButton Button) const
{
    if (Button >= MaxMouseButtons) return false;
    
    bool currentState = MouseButtons[Button];
    bool previousState = PreviousMouseButtons[Button];
    bool isPressed = currentState && !previousState;
    
    // 디버그 출력 추가
    if (bEnableDebugLogging && Button == LeftButton && (currentState || previousState))
    {
        char debugMsg[128];
        sprintf_s(debugMsg, "IsPressed: Current=%d, Previous=%d, Result=%d\n", 
                  currentState, previousState, isPressed);
        UE_LOG(debugMsg);
    }
    
    return isPressed;
}

bool UInputManager::IsMouseButtonReleased(EMouseButton Button) const
{
    if (Button >= MaxMouseButtons) return false;
    return !MouseButtons[Button] && PreviousMouseButtons[Button];
}

bool UInputManager::IsKeyDown(int KeyCode) const
{
    if (KeyCode < 0 || KeyCode >= 256) return false;
    return KeyStates[KeyCode];
}

bool UInputManager::IsKeyPressed(int KeyCode) const
{
    
    if (KeyCode < 0 || KeyCode >= 256) return false;
    return KeyStates[KeyCode] && !PreviousKeyStates[KeyCode];
}

bool UInputManager::IsKeyReleased(int KeyCode) const
{
    if (KeyCode < 0 || KeyCode >= 256) return false;
    return !KeyStates[KeyCode] && PreviousKeyStates[KeyCode];
}

void UInputManager::UpdateMousePosition(int X, int Y)
{
    MousePosition.X = static_cast<float>(X);
    MousePosition.Y = static_cast<float>(Y);
}

void UInputManager::UpdateMouseButton(EMouseButton Button, bool bPressed)
{
    if (Button < MaxMouseButtons)
    {
        MouseButtons[Button] = bPressed;
    }
}

void UInputManager::UpdateKeyState(int KeyCode, bool bPressed)
{
    if (KeyCode >= 0 && KeyCode < 256)
    {
        KeyStates[KeyCode] = bPressed;
    }
}

FVector2D UInputManager::GetScreenSize() const
{
    // 우선 ImGui 컨텍스트가 있으면 IO의 DisplaySize 사용
    if (ImGui::GetCurrentContext() != nullptr)
    {
        const ImGuiIO& io = ImGui::GetIO();
        float w = io.DisplaySize.x;
        float h = io.DisplaySize.y;
        if (w > 1.0f && h > 1.0f)
        {
            return FVector2D(w, h);
        }
    }

    // 윈도우 클라이언트 영역 쿼리
    HWND hwnd = WindowHandle ? WindowHandle : GetActiveWindow();
    if (hwnd)
    {
        RECT rc{};
        if (GetClientRect(hwnd, &rc))
        {
            float w = static_cast<float>(rc.right - rc.left);
            float h = static_cast<float>(rc.bottom - rc.top);
            if (w <= 0.0f) w = 1.0f;
            if (h <= 0.0f) h = 1.0f;
            return FVector2D(w, h);
        }
    }
    return FVector2D(1.0f, 1.0f);
}
