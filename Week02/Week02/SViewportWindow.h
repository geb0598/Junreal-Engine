#pragma once
#include "SWindow.h"
#include "FViewport.h"
#include "FViewportClient.h"

class SViewportWindow : public SWindow
{
public:
    SViewportWindow();
    virtual ~SViewportWindow();

    bool Initialize(uint32 StartX, uint32 StartY, uint32 Width, uint32 Height, UWorld* World, ID3D11Device* Device, EViewportType InViewportType);

    virtual void OnRender() override;
    virtual void OnUpdate() override;

    virtual void OnMouseMove(FVector2D MousePos) override;
    virtual void OnMouseDown(FVector2D MousePos) override;
    virtual void OnMouseUp(FVector2D MousePos) override;

    void SetActive(bool bInActive) { bIsActive = bInActive; }
    bool IsActive() const { return bIsActive; }

    FViewport* GetViewport() const { return Viewport; }
    FViewportClient* GetViewportClient() const { return ViewportClient; }

private:
    void RenderToolbar();

private:


    FViewport* Viewport = nullptr;
    FViewportClient* ViewportClient = nullptr;

    EViewportType ViewportType;
    FName ViewportName;

    bool bIsActive;
    bool bIsMouseDown;
};