#pragma once
#include "Object.h"
#include "Windows.h"
#include "Vector.h"
#include "ImGui/imgui.h"

struct ID3D11Device;
struct ID3D11DeviceContext;

class ACameraActor;
class AActor;
class AGizmoActor;

// 순환참조를 피하기 위한 전방선언
class UWorld;

class UUIManager : public UObject
{
public:
    DECLARE_CLASS(UUIManager, UObject)
    UUIManager() {}
    static UUIManager& GetInstance()
    {
        static UUIManager* Instance = nullptr;
        if (Instance == nullptr)
        {
            Instance = NewObject<UUIManager>();
        }
        return *Instance;
    }

    void Initialize(HWND hWindow, ID3D11Device* InDevice, ID3D11DeviceContext* InDeviceContext);
    void Update(FVector& OutCameraLocation);
    void BeginImGuiFrame();
    void RenderImGui();
    void EndImGuiFrame();
    void Release();


    void ResetPickedActor();
    void SetPickedActor(AActor* InPickedActor);
    AActor* GetPickedActor() const;

    void SetGizmoActor(AGizmoActor* InGizmoActor);

   // void SetGizmodActor(AActor* InPickedActor);

    void SetCamera(ACameraActor* InCameraActor);
    void ApplyAxisRotation(int Axis, float AngleDegree); // 축별 개별 회전 적용
    void UpdateMouseRotation(float Pitch, float Yaw); // 마우스 회전 동기화
    float GetStoredRoll() const { return StoredRoll; } // Roll 값 접근
    
    void SetWorld(UWorld* InWorld);

    UWorld*& GetWorld();

    void OnWindowResize(int Width, int Height);
    void SetDesignResolution(int Width, int Height);


    FVector GetTempCameraRotation() const;

    // Scene name I/O
    void SetSceneName(const char* InName);
    const char* GetSceneNameCString() const { return SceneNameBuf; }
    FString GetSceneName() const { return FString(SceneNameBuf); }

protected:
    ~UUIManager() override { Release(); }

    // 복사 생성자, 대입 연산자 금지
    UUIManager(const UUIManager&) = delete;
    UUIManager& operator=(const UUIManager&) = delete;

private:
    // 내부 유틸
    void ApplyImGuiScale(float NewScale);

private:

    // ImGui 스케일 관리용
    ImGuiStyle BaseStyle{};
    bool bStyleCaptured = false;
    float UIScale = 1.0f;
    ImVec2 DesignRes{ 1920.f, 1080.f };
    ImVec2 LastClientSize{ 0.f, 0.f };

    // 핸들/디바이스 보관(선택)
    HWND WindowHandle{};
    ID3D11Device* Device{};
    ID3D11DeviceContext* DeviceContext{};

    AActor* PickedActor{};
    AGizmoActor* GizmoActor;
    float CameraLocation[3];

    ACameraActor* CameraActor{};
    
    // UI에서 관리하는 개별 축 오일러 각도 (축별 회전으로 변환 오차 방지)
    float StoredPitch = 0.0f;  // X축 회전
    float StoredYaw = 0.0f;    // Y축 회전  
    float StoredRoll = 0.0f;   // Z축 회전
    
    float UIRotationEuler[3] = { 0.0f, 0.0f, 0.0f }; // UI 표시용

    FVector TempCameraDeg = { 0.0f, 0.0f, 0.0f };

    UWorld* WorldRef = nullptr;

    bool bReleased = false;

    // ImGui-bound scene name buffer
    char SceneNameBuf[128] = "Default";

};
