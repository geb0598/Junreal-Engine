#include "pch.h"
#include "PropertyWindow.h"
#include "../Widget/TargetActorTransformWidget.h"

UPropertyWindow::UPropertyWindow()
{
	FUIWindowConfig Config;
	Config.WindowTitle = "Property Window";
	Config.DefaultSize = ImVec2(400, 620);
	Config.DefaultPosition = ImVec2(10, 10);
	Config.MinSize = ImVec2(400, 200);
	Config.DockDirection = EUIDockDirection::Top;
	Config.Priority = 15;
	Config.bResizable = true;
	Config.bMovable = true;
	Config.bCollapsible = true;

	AddWidget(NewObject<UTargetActorTransformWidget>());

	Config.UpdateWindowFlags();
	SetConfig(Config);
}

void UPropertyWindow::Initialize()
{
	UE_LOG("UPropertyWindow: Initialized");
}
