#include "pch.h"
#include "SceneIOWidget.h"
#include "../../ObjectFactory.h"
#include "../GlobalConsole.h"
#include "../../ImGui/imgui.h"
#include <windows.h>
#include <commdlg.h>
#include <filesystem>
#include <exception>
#include "../UIManager.h"

USceneIOWidget::USceneIOWidget()
	: UWidget("Scene IO Widget")
	, StatusMessageTimer(0.0f)
	, bIsStatusError(false)
{
	strcpy_s(NewLevelNameBuffer, "NewScene");
}

USceneIOWidget::~USceneIOWidget() = default;

void USceneIOWidget::Initialize()
{
	UE_LOG("SceneIOWidget: Successfully Initialized");
}

void USceneIOWidget::Update()
{
	// Update status message timer
	if (StatusMessageTimer > 0.0f)
	{
		// Use a fixed delta time for now
		float DeltaTime = 1.0f / 60.0f; // Assume 60 FPS
		UpdateStatusMessage(DeltaTime);
	}
}

void USceneIOWidget::RenderWidget()
{
	ImGui::Text("Scene Management");
	ImGui::Separator();
	
	RenderSaveSection();
	ImGui::Spacing();
	
	RenderLoadSection();
	ImGui::Spacing();
	
	RenderNewSceneSection();
	ImGui::Spacing();
	
	RenderStatusMessage();
}

void USceneIOWidget::RenderSaveSection()
{
	ImGui::Text("Save Scene");
	
	if (ImGui::Button("Save Scene", ImVec2(100, 25)))
	{
		path FilePath = OpenSaveFileDialog();
		if (!FilePath.empty())
		{
			SaveLevel(FilePath.string());
		}
	}
	
	ImGui::SameLine();
	if (ImGui::Button("Quick Save", ImVec2(100, 25)))
	{
		// Quick save to default location
		SaveLevel("");
	}
}

void USceneIOWidget::RenderLoadSection()
{
	ImGui::Text("Load Scene");
	
	if (ImGui::Button("Load Scene", ImVec2(100, 25)))
	{
		path FilePath = OpenLoadFileDialog();
		if (!FilePath.empty())
		{
			LoadLevel(FilePath.string());
		}
	}
}

void USceneIOWidget::RenderNewSceneSection()
{
	ImGui::Text("Create New Scene");
	
	ImGui::InputText("Scene Name", NewLevelNameBuffer, sizeof(NewLevelNameBuffer));
	
	if (ImGui::Button("Create New Scene", ImVec2(150, 25)))
	{
		CreateNewLevel();
	}
}

void USceneIOWidget::RenderStatusMessage()
{
	if (StatusMessageTimer > 0.0f)
	{
		ImVec4 color = bIsStatusError ? 
			ImVec4(1.0f, 0.4f, 0.4f, 1.0f) :  // Red for errors
			ImVec4(0.0f, 1.0f, 0.0f, 1.0f);   // Green for success
			
		ImGui::TextColored(color, "%s", StatusMessage.c_str());
	}
	else
	{
		// Reserve space to prevent UI jumping
		ImGui::Text(" "); // Empty line
	}
}

void USceneIOWidget::SetStatusMessage(const FString& Message, bool bIsError)
{
	StatusMessage = Message;
	StatusMessageTimer = STATUS_MESSAGE_DURATION;
	bIsStatusError = bIsError;
}

void USceneIOWidget::UpdateStatusMessage(float DeltaTime)
{
	StatusMessageTimer -= DeltaTime;
	if (StatusMessageTimer < 0.0f)
	{
		StatusMessageTimer = 0.0f;
	}
}

/**
 * @brief Save current scene to specified file path
 * @param InFilePath File path to save to (empty for quick save)
 */
void USceneIOWidget::SaveLevel(const FString& InFilePath)
{
	try
	{
		// Get World reference
		UWorld* CurrentWorld = UUIManager::GetInstance().GetWorld();
		if (!CurrentWorld)
		{
			SetStatusMessage("Cannot find World!", true);
			return;
		}
		
		if (InFilePath.empty())
		{
			// Quick Save (QuickSave.scene)
			CurrentWorld->SaveScene("QuickSave");
			UE_LOG("SceneIO: Quick Save executed");
			SetStatusMessage("Quick Save completed successfully!");
		}
		else
		{
			// Extract scene name from file path
			FString SceneName = InFilePath;
			size_t LastSlash = SceneName.find_last_of("\\/");
			if (LastSlash != std::string::npos)
			{
				SceneName = SceneName.substr(LastSlash + 1);
			}
			size_t LastDot = SceneName.find_last_of(".");
			if (LastDot != std::string::npos)
			{
				SceneName = SceneName.substr(0, LastDot);
			}

			// Save scene through World
			CurrentWorld->SaveScene(SceneName);
			UE_LOG("SceneIO: Scene saved successfully: %s", SceneName.c_str());
			SetStatusMessage("Scene saved: " + SceneName);
		}
	}
	catch (const std::exception& Exception)
	{
		FString ErrorMsg = FString("Save Error: ") + Exception.what();
		SetStatusMessage(ErrorMsg, true);
		UE_LOG("SceneIO: Save Error: %s", Exception.what());
	}
}

/**
 * @brief Load scene from specified file path
 * @param InFilePath File path to load from
 */
void USceneIOWidget::LoadLevel(const FString& InFilePath)
{
	try
	{
		// Extract scene name from file path
		FString SceneName = InFilePath;

		// Find last slash to get filename
		size_t LastSlash = SceneName.find_last_of("\\/");
		if (LastSlash != std::string::npos)
		{
			SceneName = SceneName.substr(LastSlash + 1);
		}

		// Remove .scene extension
		size_t LastDot = SceneName.find_last_of(".");
		if (LastDot != std::string::npos)
		{
			SceneName = SceneName.substr(0, LastDot);
		}

		// Load scene through World (uses SceneLoader internally)
		UWorld* CurrentWorld = UUIManager::GetInstance().GetWorld();
		if (!CurrentWorld)
		{
			SetStatusMessage("Cannot find World!", true);
			return;
		}

		// Call World's LoadScene
		CurrentWorld->LoadScene(SceneName);
		
		UE_LOG("SceneIO: Scene loaded successfully: %s", SceneName.c_str());
		SetStatusMessage("Scene loaded successfully: " + SceneName);
	}
	catch (const std::exception& Exception)
	{
		FString ErrorMsg = FString("Load Error: ") + Exception.what();
		SetStatusMessage(ErrorMsg, true);
		UE_LOG("SceneIO: Load Error: %s", Exception.what());
	}
}

/**
 * @brief Create a new blank scene
 */
void USceneIOWidget::CreateNewLevel()
{
	try
	{
		FString LevelName = FString(NewLevelNameBuffer);

		// Validate input
		if (LevelName.empty() || LevelName == " ")
		{
			SetStatusMessage("Please enter a valid scene name!", true);
			return;
		}

		// Get World reference
		UWorld* CurrentWorld = UUIManager::GetInstance().GetWorld();
		if (!CurrentWorld)
		{
			SetStatusMessage("Cannot find World!", true);
			return;
		}

		// Create new scene through World
		CurrentWorld->CreateNewScene();
		
		UE_LOG("SceneIO: New scene created: %s", LevelName.c_str());
		SetStatusMessage("New scene created: " + LevelName);
			
		// Reset input field
		strcpy_s(NewLevelNameBuffer, "NewScene");
	}
	catch (const std::exception& Exception)
	{
		FString ErrorMsg = FString("Create Error: ") + Exception.what();
		SetStatusMessage(ErrorMsg, true);
		UE_LOG("SceneIO: Create Error: %s", Exception.what());
	}
}

/**
 * @brief Windows API를 활용한 파일 저장 Dialog Modal을 생성하는 UI Window 기능
 * @return 선택된 파일 경로 (취소 시 빈 문자열)
 */
path USceneIOWidget::OpenSaveFileDialog()
{
     OPENFILENAMEW ofn;
     wchar_t szFile[260] = {};
   
     // 기본 파일명 설정
     wcscpy_s(szFile, L"");
   
     // Initialize OPENFILENAME
     ZeroMemory(&ofn, sizeof(ofn));
     ofn.lStructSize = sizeof(ofn);
     ofn.hwndOwner = GetActiveWindow();  // 현재 활성 윈도우를 부모로 설정
     ofn.lpstrFile = szFile;
     ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
     ofn.lpstrFilter = L"JSON Files\0*.scene\0All Files\0*.*\0";
     ofn.nFilterIndex = 1;
     ofn.lpstrFileTitle = nullptr;
     ofn.nMaxFileTitle = 0;
     ofn.lpstrInitialDir = nullptr;
     ofn.lpstrTitle = L"Save Level File";
     ofn.Flags = OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT | OFN_EXPLORER | OFN_HIDEREADONLY;
     ofn.lpstrDefExt = L"json";
   
     // Modal 다이얼로그 표시 - 이 함수가 리턴될 때까지 다른 입력 차단
     UE_LOG("SceneIO: Opening Save Dialog (Modal)...");
   
     if (GetSaveFileNameW(&ofn) == TRUE)
     {
         UE_LOG("SceneIO: Save Dialog Closed");
         return path(szFile);
     }
   
    UE_LOG("SceneIO: Save Dialog Closed");
   return L"";
}

/**
 * @brief Windows API를 사용하여 파일 열기 다이얼로그를 엽니다
 * @return 선택된 파일 경로 (취소 시 빈 문자열)
 */
path USceneIOWidget::OpenLoadFileDialog()
{
    OPENFILENAMEW ofn;
    wchar_t szFile[260] = {};

    // Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = GetActiveWindow();  // 현재 활성 윈도우를 부모로 설정
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile) / sizeof(wchar_t);
    ofn.lpstrFilter = L"JSON Files\0*.scene\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = nullptr;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = nullptr;
    ofn.lpstrTitle = L"Load Level File";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_EXPLORER | OFN_HIDEREADONLY;

    UE_LOG("SceneIO: Opening Load Dialog (Modal)...");

    if (GetOpenFileNameW(&ofn) == TRUE)
    {
        UE_LOG("SceneIO: Load Dialog Closed");
        return path(szFile);
    }

    UE_LOG("SceneIO: Load Dialog Closed");
    return L"";
}
