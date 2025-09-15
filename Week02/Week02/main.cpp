#include "pch.h"
#include <fstream>

// TODO: Delete it, just Test

float CLIENTWIDTH = 1024.0f;
float CLIENTHEIGHT = 1024.0f;

// 창 크기 저장/로드 함수
void SaveWindowSize(int width, int height)
{
    std::ofstream file("window_config.txt");
    if (file.is_open())
    {
        file << width << " " << height << std::endl;
        file.close();
    }
}

bool LoadWindowSize(int& width, int& height)
{
    std::ifstream file("window_config.txt");
    if (file.is_open())
    {
        file >> width >> height;
        file.close();
        return true;
    }
    return false;
}

#if defined(_MSC_VER) && defined(_DEBUG)
#   define _CRTDBG_MAP_ALLOC
#   include <cstdlib>
#   include <crtdbg.h>
#endif
#include "UI/Factory/UIWindowFactory.h"

void GetViewportSize(HWND hWnd)
{
    RECT clientRect{};
    GetClientRect(hWnd, &clientRect);

    CLIENTWIDTH = clientRect.right - clientRect.left;
    CLIENTHEIGHT = clientRect.bottom - clientRect.top;

    if (CLIENTWIDTH <= 0) CLIENTWIDTH = 1;
    if (CLIENTHEIGHT <= 0) CLIENTHEIGHT = 1;
}

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // InputManager에 먼저 메시지 전달 (ImGui보다 먼저)
    UInputManager::GetInstance().ProcessMessage(hWnd, message, wParam, lParam);

    // ImGui 처리
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
    {
        return true;
    }

    switch (message)
    {
    case WM_SIZE:
        {
            WPARAM sizeType = wParam;
            if (sizeType != SIZE_MINIMIZED)
            {
                GetViewportSize(hWnd); // 창 크기 바뀔 때 전역 갱신
             
                // Renderer의 뷰포트 갱신
                if (auto world = UUIManager::GetInstance().GetWorld())
                {
                    if (auto renderer = world->GetRenderer())
                    {
                        UINT newWidth = static_cast<UINT>(CLIENTWIDTH);
                        UINT newHeight = static_cast<UINT>(CLIENTHEIGHT);
                        // Single, consistent resize path (handles RTV/DSV + viewport)
                        static_cast<D3D11RHI*>(renderer->GetRHIDevice())->ResizeSwapChain(newWidth, newHeight);
                    }
                    // ImGui DisplaySize가 유효할 때만 UI 윈도우 재배치
                    ImGuiIO& io = ImGui::GetIO();
                    if (io.DisplaySize.x > 0 && io.DisplaySize.y > 0)
                    {
                        UUIManager::GetInstance().RepositionImGuiWindows();
                    }
                }
            }
        }
        break;
    case WM_DESTROY:
        // Signal that the app should quit
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

void InitManager(HWND hWnd)
{
    // Renderer Class를 생성합니다.
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
#if defined(_MSC_VER) && defined(_DEBUG)
    // Enable CRT debug heap and automatic leak check on exit
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
    _CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_WARN, _CRTDBG_MODE_DEBUG);
    _CrtSetReportMode(_CRT_ASSERT, _CRTDBG_MODE_DEBUG);
    _CrtSetBreakAlloc(0); // Uncomment and set alloc ID to break on specific leak
#endif

    // _CrtSetBreakAlloc(346);

    // 윈도우 클래스 이름
    WCHAR WindowClass[] = L"JungleWindowClass";

    // 윈도우 타이틀바에 표시될 이름
    WCHAR Title[] = L"Game Tech Lab";

    // 각종 메시지를 처리할 함수인 WndProc의 함수 포인터를 WindowClass 구조체에 넣는다.
    WNDCLASSW wndclass = { 0, WndProc, 0, 0, 0, 0, 0, 0, 0, WindowClass };

    // 윈도우 클래스 등록
    RegisterClassW(&wndclass);

    // 저장된 창 크기 로드 (없으면 기본값 1024x1024)
    int windowWidth = 1024, windowHeight = 1024;
    LoadWindowSize(windowWidth, windowHeight);

    // 윈도우 생성
    HWND hWnd = CreateWindowExW(0, WindowClass, Title, WS_POPUP | WS_VISIBLE | WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, windowWidth, windowHeight,
        nullptr, nullptr, hInstance, nullptr);

    //종횡비 계산
    GetViewportSize(hWnd);

    {
        D3D11RHI d3d11RHI;
        d3d11RHI.Initialize(hWnd);
        URenderer renderer(&d3d11RHI); //렌더러 생성이 가장 먼저 되어야 합니다.

        UResourceManager::GetInstance().Initialize(d3d11RHI.GetDevice(), d3d11RHI.GetDeviceContext()); //리소스매니저 이니셜라이즈
        // UI Manager Initialize
        UUIManager::GetInstance().Initialize(hWnd, d3d11RHI.GetDevice(), d3d11RHI.GetDeviceContext()); //유아이매니저 이니셜라이즈
        UUIWindowFactory::CreateDefaultUILayout();

        // InputManager 초기화 (TUUIManager 이후)
        UInputManager::GetInstance().Initialize(hWnd); //인풋 매니저 이니셜라이즈

        //======================================================================================================================      

        UWorld* World = NewObject<UWorld>();
        World->SetRenderer(&renderer);
        World->Initialize(); //월드 생성 


        //스폰을 위한 월드셋
        UUIManager::GetInstance().SetWorld(World);
        //======================================================================================================================      
        LARGE_INTEGER Frequency;
        QueryPerformanceFrequency(&Frequency);

        LARGE_INTEGER PrevTime, CurrTime;
        QueryPerformanceCounter(&PrevTime);

        FVector CameraLocation{ 0, 0, -10.f };

        UInputManager& InputMgr = UInputManager::GetInstance();

        bool bIsExit = false;
        while (bIsExit == false)
        {
            MSG msg;

            QueryPerformanceCounter(&CurrTime);

            // 프레임 간 시간 (초 단위)
            float DeltaSeconds = static_cast<float>(
                (CurrTime.QuadPart - PrevTime.QuadPart) / double(Frequency.QuadPart)
                );
            PrevTime = CurrTime;


            // 이제 Tick 호출
            World->Tick(DeltaSeconds);
            World->Render();
            //UE_LOG("Hello World %d", 2025);

            // 처리할 메시지가 더 이상 없을때 까지 수행
            while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);

                if (msg.message == WM_QUIT)
                {
                    bIsExit = true;
                    break;
                }
            }

            if (InputMgr.IsKeyPressed(VK_ESCAPE))
            {
                UE_LOG("ESC Key Pressed - Exiting!\n");
                bIsExit = true;
            }

            // 마우스 위치 실시간 출력 (매 60프레임마다)
            static int frameCount = 0;
            frameCount++;
            if (frameCount % 60 == 0) // 60프레임마다 출력
            {
                FVector2D mousePos = InputMgr.GetMousePosition();
                FVector2D mouseDelta = InputMgr.GetMouseDelta();
                char debugMsg[128];
                //sprintf_s(debugMsg, "Mouse Pos: (%.1f, %.1f), Delta: (%.1f, %.1f)\n",
                //          mousePos.X, mousePos.Y, mouseDelta.X, mouseDelta.Y);
                //UE_LOG(debugMsg);
            }
        }

        // 전역 변수 CLIENTWIDTH/HEIGHT를 사용해서 저장 (더 안전)
        if (CLIENTWIDTH > 100 && CLIENTHEIGHT > 100)
        {
            // 클라이언트 영역에 프레임 사이즈 추가해서 전체 창 크기 계산
            int totalWidth = (int)CLIENTWIDTH + 16;  // 대략적인 좌우 프레임
            int totalHeight = (int)CLIENTHEIGHT + 39; // 대략적인 타이틀바 + 상하 프레임
            SaveWindowSize(totalWidth, totalHeight);
        }
        else
        {
            printf("Invalid client size (%.0f x %.0f), not saving\n", CLIENTWIDTH, CLIENTHEIGHT);
        }
        UUIManager::GetInstance().Release();
        ObjectFactory::DeleteAll(true);
    }

    return 0;
}
