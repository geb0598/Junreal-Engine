#pragma once
#include "Widget.h"

class UUIManager;
class AActor;

class UActorTerminationWidget
	: public UWidget
{
public:
	DECLARE_CLASS(UActorTerminationWidget, UWidget)

	void Initialize() override;
	void Update() override;
	void RenderWidget() override;
	void DeleteSelectedActor();

	UActorTerminationWidget();
	~UActorTerminationWidget() override;

private:
	AActor* SelectedActor;
	UUIManager* UIManager; // UIManager 참조
};
