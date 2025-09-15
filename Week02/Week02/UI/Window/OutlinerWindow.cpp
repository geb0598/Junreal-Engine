#include "pch.h"
#include "OutlinerWindow.h"
#include "../Widget/WorldOutlinerWidget.h"
#include "../Widget/ActorTerminationWidget.h"
#include "../Widget/TargetActorTransformWidget.h"

// UE_LOG 대체 매크로
#define UE_LOG(fmt, ...)

UOutlinerWindow::UOutlinerWindow()
{
	FUIWindowConfig Config;
	Config.WindowTitle = "Scene Manager";
	Config.DefaultSize = ImVec2(350, 500);
	Config.DefaultPosition = ImVec2(1225, 10);
	Config.MinSize = ImVec2(300, 400);
	Config.bResizable = true;
	Config.bMovable = true;
	Config.bCollapsible = true;
	Config.DockDirection = EUIDockDirection::Center;

	Config.UpdateWindowFlags();
	SetConfig(Config);

	// Add World Outliner Widget (main scene hierarchy)
	UWorldOutlinerWidget* OutlinerWidget = NewObject<UWorldOutlinerWidget>();
	if (OutlinerWidget)
	{
		AddWidget(OutlinerWidget);
		UE_LOG("OutlinerWindow: WorldOutlinerWidget created successfully");
	}
	else
	{
		UE_LOG("OutlinerWindow: Failed to create WorldOutlinerWidget");
	}
	
	// Add Transform Widget for selected actor
	UTargetActorTransformWidget* TransformWidget = NewObject<UTargetActorTransformWidget>();
	if (TransformWidget)
	{
		AddWidget(TransformWidget);
		UE_LOG("OutlinerWindow: TargetActorTransformWidget created successfully");
	}
	
	// Add Actor Termination Widget
	UActorTerminationWidget* TerminationWidget = NewObject<UActorTerminationWidget>();
	if (TerminationWidget)
	{
		AddWidget(TerminationWidget);
		UE_LOG("OutlinerWindow: ActorTerminationWidget created successfully");
	}
}

void UOutlinerWindow::Initialize()
{
	UE_LOG("OutlinerWindow: Successfully Initialized");
}
