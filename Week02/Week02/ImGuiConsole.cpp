#include "ImGuiConsole.h"
#include <Windows.h>

ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
void* GImGuiDemoMarkerCallbackUserData = NULL;

// 전역 콘솔 포인터
static ExampleAppConsole* GConsoleInstance = nullptr;

void ImGuiConsole::ShowExampleAppConsole(bool* p_open)
{
    static ExampleAppConsole console;
    GConsoleInstance = &console;
    console.Draw("Example: Console", p_open);
}

void ImGuiConsole::Shutdown()
{
    if (GConsoleInstance)
    {
        // 해제: 로그와 히스토리 모두 반환
        GConsoleInstance->ClearLog();
        for (int i = 0; i < GConsoleInstance->History.Size; ++i)
            ImGui::MemFree(GConsoleInstance->History[i]);
        GConsoleInstance->History.clear();
        GConsoleInstance = nullptr;
    }
}

void ConsoleLogV(const char* fmt, va_list args)
{
    if (GConsoleInstance)
    {
        GConsoleInstance->VAddLog(fmt, args);
    }
    else
    {
        // 콘솔 아직 안 떠있을 때 대비(옵션): 디버그 출력으로라도 흘려보냄
        char tmp[1024];
        vsnprintf_s(tmp, _countof(tmp), fmt, args);
        OutputDebugStringA(tmp);
        OutputDebugStringA("\n");
    }

}

void ConsoleLog(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    ConsoleLogV(fmt, args);
    va_end(args);
}
