#pragma once
#include"Engine.h"

class UGameEngine :
    public UEngine
{
public:
    UGameEngine();
    ~UGameEngine() override;
    DECLARE_CLASS(UGameEngine, UEngine)
    UWorld* GameWorld = nullptr;

    virtual void Tick(float DeltaSeconds) override;

    void StartGame(UWorld* World);
    void EndGame();
protected:
    
};

