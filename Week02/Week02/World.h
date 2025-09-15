#pragma once
#include "GizmoActor.h"
#include "Object.h"
#include "GridActor.h"
#include "GizmoActor.h"

// Forward Declarations
class UResourceManager;
class UUIManager;
class UInputManager;
class USelectionManager;
class AActor;
class URenderer;
class ACameraActor;
class AGizmoActor;
struct FTransform;
struct FPrimitiveData;


/**
 * UWorld
 * - 월드 단위의 액터/타임/매니저 관리 클래스
 */
class UWorld final : public UObject
{
public:
    DECLARE_CLASS(UWorld, UObject)
    UWorld();

protected:
    ~UWorld() override;

public:
    /** 초기화 */
    void Initialize();
	void InitializeMainCamera();
    void InitializeGizmo();
    void SetRenderer(URenderer* InRenderer);
    URenderer*& const GetRenderer()  { return Renderer; }
    template<class T>
    T* SpawnActor(const FTransform& Transform);

    void AddActor(AActor* Actor)
    {
        Actors.Add(Actor);
    }

    bool DestroyActor(AActor* Actor);

    void CreateNewScene();
    void LoadScene(const FString& SceneName);
    void SaveScene(const FString& SceneName);
    ACameraActor* GetCameraActor() { return MainCameraActor; }


    const TArray<AActor*>& GetActors() { return Actors; }

    /** === 타임 / 틱 === */
    virtual void Tick(float DeltaSeconds);
    float GetTimeSeconds() const;

    /** === 렌더 === */
    void Render();

    
    /** === 레벨 / 월드 구성 === */
    // TArray<ULevel*> Levels;

    /** === 플레이어 / 컨트롤러 === */
    // APlayerController* GetFirstPlayerController() const;
    // TArray<APlayerController*> GetPlayerControllerIterator() const;

private:
    // 싱글톤 매니저 참조
    UResourceManager& ResourceManager;
    UUIManager& UIManager;
    UInputManager& InputManager;
    USelectionManager& SelectionManager;

   
    // 메인 카메라
    ACameraActor* MainCameraActor = nullptr;

    AGridActor* GridActor = nullptr;
    // 렌더러 (월드가 소유)
    URenderer* Renderer;

    TArray<FPrimitiveData> Primitives;

    /** === 액터 관리 === */
    TArray<AActor*> Actors;

    /** == 기즈모 == */
    AGizmoActor* GizmoActor;

    /** === 에디터 카메라 조작 === */
    void ProcessEditorCameraInput(float DeltaSeconds);
    void ProcessCameraRotation(float DeltaSeconds);
    void ProcessCameraMovement(float DeltaSeconds);
    
    // 카메라 설정
    float MouseSensitivity = 0.1f;  // 0.005f -> 0.1f로 증가 (20배)
    float CameraMoveSpeed = 5.0f;

    bool bIsWireframeMode = false;
    EViewModeIndex ViewModeIndex = EViewModeIndex::VMI_Wireframe;
};

template<class T>
inline T* UWorld::SpawnActor(const FTransform& Transform)
{
    static_assert(std::is_base_of<AActor, T>::value, "T must be derived from AActor");

    // 새 액터 생성
    T* NewActor = NewObject<T>();

    // 초기 트랜스폼 적용
    NewActor->SetActorTransform(Transform);

    //  월드 등록
    NewActor->SetWorld(this);

    // 월드에 등록
    Actors.Add(NewActor);

    return NewActor;
}
