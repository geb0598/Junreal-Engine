#include "pch.h"
#include "OutlinerWindow.h"
#include "../Widget/ActorTerminationWidget.h"
#include "../Widget/TargetActorTransformWidget.h"

// UE_LOG 대체 매크로
#define UE_LOG(fmt, ...)

UOutlinerWindow::UOutlinerWindow()
{
	FUIWindowConfig Config;
	Config.WindowTitle = "Outliner";
	Config.DefaultSize = ImVec2(350, 280);
	Config.DefaultPosition = ImVec2(1225, 10);
	Config.MinSize = ImVec2(350, 280);
	Config.bResizable = true;
	Config.bMovable = true;
	Config.bCollapsible = true;
	Config.DockDirection = EUIDockDirection::Center;

	Config.UpdateWindowFlags();
	SetConfig(Config);

	AddWidget(NewObject<UTargetActorTransformWidget>());
	AddWidget(NewObject<UActorTerminationWidget>());
}

void UOutlinerWindow::Initialize()
{
	UE_LOG("OutlinerWindow: Successfully Initialized");
}
