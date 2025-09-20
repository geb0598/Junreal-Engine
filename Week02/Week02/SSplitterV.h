#pragma once
#include "SSplitter.h"
/**
 * @brief 수직 스플리터 (상/하 분할)
 */
class SSplitterV : public SSplitter
{
public:
    SSplitterV() = default;
    
    void UpdateDrag(FVector2D MousePos) override;

protected:
    ~SSplitterV() override;
    void UpdateChildRects() override;
    FRect GetSplitterRect() const override;
    ImGuiMouseCursor GetMouseCursor() const override { return ImGuiMouseCursor_ResizeNS; }
};

