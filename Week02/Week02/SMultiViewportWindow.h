#pragma once
#include "SWindow.h"
#include "SSplitterV.h"
#include "SSplitterH.h"
#include "SViewportWindow.h"

class SMultiViewportWindow : public SWindow
{
public:
    SMultiViewportWindow();
    virtual ~SMultiViewportWindow();

    void Initialize(ID3D11Device* Device, UWorld* World, const FRect& InRect);

    virtual void OnRender() override;
    virtual void OnUpdate() override;
    virtual void OnMouseMove(FVector2D MousePos) override;
    virtual void OnMouseDown(FVector2D MousePos) override;
    virtual void OnMouseUp(FVector2D MousePos) override;

private:
    UWorld* World = nullptr;
    SSplitterV* RootSplitter = nullptr;
    SViewportWindow* Viewports[4];
};
