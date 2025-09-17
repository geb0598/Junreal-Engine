#pragma once
#include "GizmoActor.h"
#include "Object.h"
#include "GridActor.h"
#include "GizmoActor.h"
#include "Enums.h"

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
    void InitializeGrid();
    void InitializeGizmo();
    
    // 액터 인터페이스 관리
    void SetupActorReferences();
    void UpdateActorInteractions(float DeltaSeconds);
    
    // 선택 및 피킹 처리
    void ProcessActorSelection();

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

    EViewModeIndex GetViewModeIndex() { return ViewModeIndex; }
    void SetViewModeIndex(EViewModeIndex InViewModeIndex) { ViewModeIndex = InViewModeIndex; }

    /** === Show Flag 시스템 === */
    EEngineShowFlags GetShowFlags() const { return ShowFlags; }
    void SetShowFlags(EEngineShowFlags InShowFlags) { ShowFlags = InShowFlags; }
    void EnableShowFlag(EEngineShowFlags Flag) { ShowFlags |= Flag; }
    void DisableShowFlag(EEngineShowFlags Flag) { ShowFlags &= ~Flag; }
    void ToggleShowFlag(EEngineShowFlags Flag) { ShowFlags = HasShowFlag(ShowFlags, Flag) ? (ShowFlags & ~Flag) : (ShowFlags | Flag); }
    bool IsShowFlagEnabled(EEngineShowFlags Flag) const { return HasShowFlag(ShowFlags, Flag); }

    AGridActor* GetGridActor() { return GridActor; }

    const TArray<AActor*>& GetActors() { return Actors; }
    
    /** Generate unique name for actor based on type */
    FString GenerateUniqueActorName(const FString& ActorType);

    /** === 타임 / 틱 === */
    virtual void Tick(float DeltaSeconds);
    float GetTimeSeconds() const;

    /** === 렌더 === */
    void Render();
    void RenderGizmoActor();

    
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
    TArray<AActor*> EngineActors;
    /** === 액터 관리 === */
    TArray<AActor*> Actors;
    
    // Object naming system
    std::map<FString, int32> ObjectTypeCounts;
    
    /** == 기즈모 == */
    AGizmoActor* GizmoActor;

    /** === Show Flag 시스템 === */
    EEngineShowFlags ShowFlags = EEngineShowFlags::SF_DefaultEnabled;
    
    EViewModeIndex ViewModeIndex = EViewModeIndex::VMI_Unlit;
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
