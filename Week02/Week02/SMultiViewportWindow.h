#pragma once
#include "SWindow.h"
#include "SSplitterV.h"
#include "SSplitterH.h"
#include "SViewportWindow.h"

class SSceneIOWindow; // 새로 추가할 UI
class SDetailsWindow;
class SMultiViewportWindow : public SWindow
{
public:
    SMultiViewportWindow();
    virtual ~SMultiViewportWindow();

    void Initialize(ID3D11Device* Device, UWorld* World, const FRect& InRect, SViewportWindow* MainViewport);
    void SwitchLayout(EViewportLayoutMode NewMode);


    virtual void OnRender() override;
    virtual void OnUpdate() override;
    virtual void OnMouseMove(FVector2D MousePos) override;
    virtual void OnMouseDown(FVector2D MousePos) override;
    virtual void OnMouseUp(FVector2D MousePos) override;


    void OnShutdown();
private:
    UWorld* World = nullptr;
    ID3D11Device* Device = nullptr;

    SSplitterH* RootSplitter = nullptr;

    // 두 가지 레이아웃을 미리 생성해둠
    SSplitter* FourSplitLayout = nullptr;
    SSplitter* SingleLayout = nullptr;

    // 뷰포트
    SViewportWindow* Viewports[4];
    SViewportWindow* MainViewport;
 
    // 오른쪽 고정 UI
    SWindow* SceneIOPanel = nullptr;
    // 아래쪽 UI
    SWindow* ControlPanel = nullptr;
    SWindow* DetailPanel = nullptr;

    SSplitterV* TopPanel = nullptr;
    SSplitterV* LeftPanel = nullptr;
    
    // 현재 모드
    EViewportLayoutMode CurrentMode = EViewportLayoutMode::FourSplit;

};
