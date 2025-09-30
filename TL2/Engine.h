#pragma once
#include "Object.h"
#
#include "World.h"

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
    DECLARE_CLASS(UEngine,UObject)
    
    TArray<FWorldContext> WorldContexts;

    UWorld* GetWorld(EWorldType Type);
};

