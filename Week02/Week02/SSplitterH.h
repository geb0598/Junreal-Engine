#pragma once
#include "SSplitter.h"
/**
 * @brief 수평 스플리터 (좌/우 분할)
 */
class SSplitterH : public SSplitter
{
public:
    SSplitterH() = default;

    void UpdateDrag(FVector2D MousePos) override;

protected:
    ~SSplitterH() override;
    void UpdateChildRects() override;
    FRect GetSplitterRect() const override;
    ImGuiMouseCursor GetMouseCursor() const override { return ImGuiMouseCursor_ResizeEW; }
};