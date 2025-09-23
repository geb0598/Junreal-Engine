#pragma once
#include "SWindow.h"
#include "SSplitterV.h"
#include "SSplitterH.h"
#include "SViewportWindow.h"

class SSceneIOWindow; // 새로 추가할 UI
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


    void OnShutdown();
private:
    // 오른쪽 고정 UI
    SSceneIOWindow* SceneIOPanel = nullptr;
    // 아래쪽 UI
    SSceneIOWindow* ConsolePanel = nullptr;
    SSceneIOWindow* PropertyPanel = nullptr;
    UWorld* World = nullptr;
    SSplitterH* RootSplitter = nullptr;
    SViewportWindow* Viewports[4];



};
