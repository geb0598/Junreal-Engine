﻿#include "pch.h"
#include "ConsoleWidget.h"
#include "../../ObjectFactory.h"
#include "../GlobalConsole.h"
#include "../StatsOverlayD2D.h"
#include <windows.h>
#include <cstdarg>
#include <cctype>
#include <cstring>
#include <algorithm>

using std::max;
using std::min;

UConsoleWidget::UConsoleWidget()
    : UWidget("Console Widget")
    , HistoryPos(-1)
    , AutoScroll(true)
    , ScrollToBottom(false)
{
    memset(InputBuf, 0, sizeof(InputBuf));
}

UConsoleWidget::~UConsoleWidget()
{
	// Clear the GlobalConsole pointer to prevent dangling pointer access
	if (UGlobalConsole::GetConsoleWidget() == this)
	{
		UGlobalConsole::SetConsoleWidget(nullptr);
	}
}

void UConsoleWidget::Initialize()
{
    ClearLog();
    
    // Default commands
    Commands.Add("HELP");
    Commands.Add("HISTORY");  
    Commands.Add("CLEAR");
    Commands.Add("CLASSIFY");
    Commands.Add("STAT");
    Commands.Add("STAT FPS");
    Commands.Add("STAT MEMORY");
    Commands.Add("STAT NONE");
    
    // Add welcome messages
    AddLog("=== Console Widget Initialized ===");
    AddLog("Console initialized. Type 'HELP' for available commands.");
    AddLog("Testing console output functionality...");
    
    // Test UE_LOG after ConsoleWidget is created
    UE_LOG("ConsoleWidget: Successfully Initialized");
    UE_LOG("This message should appear in the console widget");
}

void UConsoleWidget::Update()
{
    // Console doesn't need per-frame updates
}

void UConsoleWidget::RenderWidget()
{
    // Show basic info at top
    ImGui::Text("Console - %d messages", Items.Num());
    ImGui::Separator();
    
    // Main console area
    RenderToolbar();
    ImGui::Separator();
    RenderLogOutput();
    ImGui::Separator();
    RenderCommandInput();
}

void UConsoleWidget::RenderToolbar()
{
    if (ImGui::SmallButton("Add Debug Text")) 
    { 
        AddLog("%d some text", Items.Num()); 
        AddLog("some more text"); 
        AddLog("display very important message here!"); 
    }
    ImGui::SameLine();
    
    if (ImGui::SmallButton("Add Debug Error")) 
    { 
        AddLog("[error] something went wrong"); 
    }
    ImGui::SameLine();
    
    if (ImGui::SmallButton("Clear")) 
    { 
        ClearLog(); 
    }
    ImGui::SameLine();
    
    bool copy_to_clipboard = ImGui::SmallButton("Copy");
    
    // Options menu
    ImGui::SameLine();
    if (ImGui::Button("Options"))
        ImGui::OpenPopup("Options");
        
    if (ImGui::BeginPopup("Options"))
    {
        ImGui::Checkbox("Auto-scroll", &AutoScroll);
        ImGui::EndPopup();
    }
    
    ImGui::SameLine();
    Filter.Draw("Filter", 180);
}

void UConsoleWidget::RenderLogOutput()
{
    // Reserve space for input at bottom
    const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
    
    if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), 
                          ImGuiChildFlags_NavFlattened, ImGuiWindowFlags_HorizontalScrollbar))
    {
        if (ImGui::BeginPopupContextWindow())
        {
            if (ImGui::Selectable("Clear")) ClearLog();
            ImGui::EndPopup();
        }

        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing
        
        for (const FString& item : Items)
        {
            if (!Filter.PassFilter(item.c_str()))
                continue;

            // Color coding for different log levels
            ImVec4 color;
            bool has_color = false;
            
            if (item.find("[error]") != std::string::npos) 
            { 
                color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); 
                has_color = true; 
            }
            else if (item.find("[warning]") != std::string::npos) 
            { 
                color = ImVec4(1.0f, 0.8f, 0.0f, 1.0f); 
                has_color = true; 
            }
            else if (item.find("[info]") != std::string::npos) 
            { 
                color = ImVec4(0.0f, 0.8f, 1.0f, 1.0f); 
                has_color = true; 
            }
            
            if (has_color)
                ImGui::PushStyleColor(ImGuiCol_Text, color);
            ImGui::TextUnformatted(item.c_str());
            if (has_color)
                ImGui::PopStyleColor();
        }

        // Auto scroll to bottom
        if (ScrollToBottom || (AutoScroll && ImGui::GetScrollY() >= ImGui::GetScrollMaxY()))
            ImGui::SetScrollHereY(1.0f);
        ScrollToBottom = false;

        ImGui::PopStyleVar();
    }
    ImGui::EndChild();
}

void UConsoleWidget::RenderCommandInput()
{
    // Command input
    bool reclaim_focus = false;
    ImGuiInputTextFlags input_text_flags = ImGuiInputTextFlags_EnterReturnsTrue | 
                                          ImGuiInputTextFlags_EscapeClearsAll | 
                                          ImGuiInputTextFlags_CallbackCompletion | 
                                          ImGuiInputTextFlags_CallbackHistory;
                                          
    if (ImGui::InputText("Input", InputBuf, sizeof(InputBuf), input_text_flags, &TextEditCallbackStub, (void*)this))
    {
        char* s = InputBuf;
        Strtrim(s);
        if (s[0])
            ExecCommand(s);
        strcpy_s(InputBuf, sizeof(InputBuf), "");
        reclaim_focus = true;
    }

    // Auto-focus
    ImGui::SetItemDefaultFocus();
    if (reclaim_focus)
        ImGui::SetKeyboardFocusHere(-1);
}

void UConsoleWidget::AddLog(const char* fmt, ...)
{
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf_s(buf, sizeof(buf), fmt, args);
    buf[sizeof(buf) - 1] = 0;
    va_end(args);
    
    Items.Add(FString(buf));
    ScrollToBottom = true;
}

void UConsoleWidget::VAddLog(const char* fmt, va_list args)
{
    char buf[1024];
    vsnprintf_s(buf, sizeof(buf), fmt, args);
    buf[sizeof(buf) - 1] = 0;
    
    Items.Add(FString(buf));
    ScrollToBottom = true;
}

void UConsoleWidget::ClearLog()
{
    Items.Empty();
}

void UConsoleWidget::ExecCommand(const char* command_line)
{
    AddLog("# %s", command_line);

    // Add to history
    HistoryPos = -1;
    for (int32 i = History.Num() - 1; i >= 0; i--)
    {
        if (Stricmp(History[i].c_str(), command_line) == 0)
        {
            History.RemoveAt(i);
            break;
        }
    }
    History.Add(FString(command_line));

    // Process command
    if (Stricmp(command_line, "CLEAR") == 0)
    {
        ClearLog();
    }
    else if (Stricmp(command_line, "HELP") == 0)
    {
        AddLog("Commands:");
        for (const FString& cmd : Commands)
            AddLog("- %s", cmd.c_str());
    }
    else if (Stricmp(command_line, "HISTORY") == 0)
    {
        int32 first = History.Num() - 10;
        for (int32 i = max(0, first); i < History.Num(); i++)
            AddLog("%3d: %s", i, History[i].c_str());
    }
    else if (Stricmp(command_line, "CLASSIFY") == 0)
    {
        AddLog("This is a classification test command.");
    }
    else if (Stricmp(command_line, "STAT") == 0)
    {
        AddLog("STAT commands:");
        AddLog("- STAT FPS");
        AddLog("- STAT MEMORY");
        AddLog("- STAT NONE");
    }
    else if (Stricmp(command_line, "STAT FPS") == 0)
    {
        UStatsOverlayD2D::Get().SetShowFPS(true);
        AddLog("STAT FPS: ON");
    }
    else if (Stricmp(command_line, "STAT MEMORY") == 0)
    {
        UStatsOverlayD2D::Get().SetShowMemory(true);
        AddLog("STAT MEMORY: ON");
    }
    else if (Stricmp(command_line, "STAT NONE") == 0)
    {
        UStatsOverlayD2D::Get().SetShowFPS(false);
        UStatsOverlayD2D::Get().SetShowMemory(false);
        AddLog("STAT: OFF");
    }
    else
    {
        AddLog("Unknown command: '%s'", command_line);
    }

    ScrollToBottom = true;
}

// Static helper methods
int UConsoleWidget::Stricmp(const char* s1, const char* s2)
{
    int d;
    while ((d = toupper(*s2) - toupper(*s1)) == 0 && *s1) 
    { 
        s1++; 
        s2++; 
    }
    return d;
}

int UConsoleWidget::Strnicmp(const char* s1, const char* s2, int n)
{
    int d = 0;
    while (n > 0 && (d = toupper(*s2) - toupper(*s1)) == 0 && *s1) 
    { 
        s1++; 
        s2++; 
        n--; 
    }
    return d;
}

void UConsoleWidget::Strtrim(char* s)
{
    char* str_end = s + strlen(s);
    while (str_end > s && str_end[-1] == ' ') 
        str_end--;
    *str_end = 0;
}

int UConsoleWidget::TextEditCallbackStub(ImGuiInputTextCallbackData* data)
{
    UConsoleWidget* console = (UConsoleWidget*)data->UserData;
    return console->TextEditCallback(data);
}

int UConsoleWidget::TextEditCallback(ImGuiInputTextCallbackData* data)
{
    switch (data->EventFlag)
    {
    case ImGuiInputTextFlags_CallbackCompletion:
        {
            // Find word boundaries
            const char* word_end = data->Buf + data->CursorPos;
            const char* word_start = word_end;
            while (word_start > data->Buf)
            {
                const char c = word_start[-1];
                if (c == ' ' || c == '\t' || c == ',' || c == ';')
                    break;
                word_start--;
            }

            // Build candidates list
            TArray<FString> candidates;
            for (const FString& cmd : Commands)
            {
                if (Strnicmp(cmd.c_str(), word_start, (int)(word_end - word_start)) == 0)
                    candidates.Add(cmd);
            }

            if (candidates.Num() == 0)
            {
                AddLog("No match for \"%.*s\"!", (int)(word_end - word_start), word_start);
            }
            else if (candidates.Num() == 1)
            {
                // Single match - complete it
                data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                data->InsertChars(data->CursorPos, candidates[0].c_str());
                data->InsertChars(data->CursorPos, " ");
            }
            else
            {
                // Multiple matches - show them
                int match_len = (int)(word_end - word_start);
                for (;;)
                {
                    int c = 0;
                    bool all_candidates_matches = true;
                    for (int32 i = 0; i < candidates.Num() && all_candidates_matches; i++)
                    {
                        if (i == 0)
                            c = toupper(candidates[i][match_len]);
                        else if (c == 0 || c != toupper(candidates[i][match_len]))
                            all_candidates_matches = false;
                    }
                    if (!all_candidates_matches)
                        break;
                    match_len++;
                }

                if (match_len > 0)
                {
                    data->DeleteChars((int)(word_start - data->Buf), (int)(word_end - word_start));
                    data->InsertChars(data->CursorPos, candidates[0].c_str(), candidates[0].c_str() + match_len);
                }

                AddLog("Possible matches:");
                for (const FString& candidate : candidates)
                    AddLog("- %s", candidate.c_str());
            }
            break;
        }
    case ImGuiInputTextFlags_CallbackHistory:
        {
            const int32 prev_history_pos = HistoryPos;
            if (data->EventKey == ImGuiKey_UpArrow)
            {
                if (HistoryPos == -1)
                    HistoryPos = History.Num() - 1;
                else if (HistoryPos > 0)
                    HistoryPos--;
            }
            else if (data->EventKey == ImGuiKey_DownArrow)
            {
                if (HistoryPos != -1)
                    if (++HistoryPos >= History.Num())
                        HistoryPos = -1;
            }

            if (prev_history_pos != HistoryPos)
            {
                const char* history_str = (HistoryPos >= 0) ? History[HistoryPos].c_str() : "";
                data->DeleteChars(0, data->BufTextLen);
                data->InsertChars(0, history_str);
            }
            break;
        }
    }
    return 0;
}
