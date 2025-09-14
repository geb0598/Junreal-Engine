#include "pch.h"
#include "ActorTerminationWidget.h"
#include "../UIManager.h"
#include "../../ImGui/imgui.h"
#include "../../Actor.h"
#include "../../InputManager.h"
#include "../../World.h"

// UE_LOG 대체 매크로
#define UE_LOG(fmt, ...)

UActorTerminationWidget::UActorTerminationWidget()
	: UWidget("Actor Termination Widget")
	, SelectedActor(nullptr)
	, UIManager(&UUIManager::GetInstance())
{
}

UActorTerminationWidget::~UActorTerminationWidget() = default;

void UActorTerminationWidget::Initialize()
{
	// UIManager 참조 확보
	UIManager = &UUIManager::GetInstance();
}

void UActorTerminationWidget::Update()
{
	// UIManager를 통해 현재 선택된 액터 가져오기
	if (UIManager)
	{
		AActor* CurrentSelectedActor = UIManager->GetSelectedActor();
		
		// Update Current Selected Actor
		if (SelectedActor != CurrentSelectedActor)
		{
			SelectedActor = CurrentSelectedActor;
		}
	}
}

void UActorTerminationWidget::RenderWidget()
{
	auto& InputManager = UInputManager::GetInstance();

	if (SelectedActor)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.6f, 0.6f, 1.0f), "Selected: %s (%p)",
		                   SelectedActor->GetName().c_str(), SelectedActor);

		if (ImGui::Button("Delete Selected") || InputManager.IsKeyPressed(VK_DELETE))
		{
			DeleteSelectedActor();
		}
	}
	else
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Actor Selected For Deletion");
	}
}

/**
 * @brief Selected Actor 삭제 함수
 */
void UActorTerminationWidget::DeleteSelectedActor()
{
	if (!SelectedActor)
	{
		UE_LOG("ActorTerminationWidget: No Actor Selected For Deletion");
		return;
	}

	if (!UIManager)
	{
		UE_LOG("ActorTerminationWidget: UIManager not available");
		return;
	}

	// UIManager를 통해 World에 접근
	UWorld* World = UIManager->GetWorld();
	if (!World)
	{
		UE_LOG("ActorTerminationWidget: No World available for deletion");
		return;
	}

	UE_LOG("ActorTerminationWidget: Deleting Selected Actor: %s (%p)",
	       SelectedActor->GetName().empty() ? "UnNamed" : SelectedActor->GetName().c_str(),
	       SelectedActor);

	// World를 통해 액터 삭제
	if (World->DestroyActor(SelectedActor))
	{
		// UIManager의 선택 상태도 초기화
		UIManager->ResetPickedActor();
		SelectedActor = nullptr;
		UE_LOG("ActorTerminationWidget: Actor successfully deleted");
	}
	else
	{
		UE_LOG("ActorTerminationWidget: Failed to delete actor");
	}
}
