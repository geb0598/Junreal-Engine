#pragma once
#include "Object.h"
class UWorld;

extern UWorld* GWorld;

enum class EWorldType
{
    None,
    Editor,
    PIE,
    Game
};

struct FWorldContext
{
    EWorldType WorldType = EWorldType::None;
    UWorld* CurrentWorld = nullptr;

    UWorld* World() { return CurrentWorld; }
    const UWorld* World() const { return CurrentWorld; }

    void SetWorld(UWorld* InWorld, EWorldType InType)
    {
        CurrentWorld = InWorld;
        WorldType = InType;
    }
};
class UEngine :
    public UObject
{
public:
    UEngine();
 
    DECLARE_CLASS(UEngine,UObject)
    
    TArray<FWorldContext> WorldContexts;
    //활성화된 월드 가져오기 
    UWorld* GetWorld();

    // 현재 활성 월드 반환
    UWorld* GetActiveWorld();

    virtual void SetWorld(UWorld* InWorld, EWorldType InType);
    // 공통 Tick
    virtual void Tick(float DeltaSeconds);
    virtual void Render();
protected:
    ~UEngine() override;
};

