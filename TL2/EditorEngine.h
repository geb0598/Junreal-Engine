#pragma once
#include"Engine.h"

class UGameEngine;
class UEditorEngine :
    public UEngine
{
public:
    DECLARE_CLASS(UEditorEngine, UEngine)
    UEditorEngine();
    UGameEngine* GameEngine = nullptr; // PIE 실행용
    virtual void Tick(float DeltaSeconds) override;
    virtual void Render() override;
    void StartPIE();
    void EndPIE();
protected:
    ~UEditorEngine();
private:



};

