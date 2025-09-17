#include "pch.h"
#include "TargetActorTransformWidget.h"
#include "UI/UIManager.h"
#include "ImGui/imgui.h"
#include "Actor.h"
#include "World.h"
#include "Vector.h"
#include "GizmoActor.h"
#include <string>

using namespace std;

// UE_LOG 대체 매크로
#define UE_LOG(fmt, ...)

UTargetActorTransformWidget::UTargetActorTransformWidget()
	: UWidget("Target Actor Transform Widget")
	, UIManager(&UUIManager::GetInstance())
{

}

UTargetActorTransformWidget::~UTargetActorTransformWidget() = default;

void UTargetActorTransformWidget::Initialize()
{
	// UIManager 참조 확보
	UIManager = &UUIManager::GetInstance();
	
	// GizmoActor 참조 획득
	GizmoActor = UIManager->GetGizmoActor();
	
	// 초기 기즈모 스페이스 모드 설정
	if (GizmoActor)
	{
		CurrentGizmoSpace = GizmoActor->GetSpace();
	}
}

AActor* UTargetActorTransformWidget::GetCurrentSelectedActor() const
{
	if (!UIManager)
		return nullptr;
		
	return UIManager->GetSelectedActor();
}

void UTargetActorTransformWidget::Update()
{
	// UIManager를 통해 현재 선택된 액터 가져오기
	AActor* CurrentSelectedActor = GetCurrentSelectedActor();
	if (SelectedActor != CurrentSelectedActor)
	{
		SelectedActor = CurrentSelectedActor;
		// 새로 선택된 액터의 이름 캐시
		if (SelectedActor)
		{
			try
			{
				CachedActorName = SelectedActor->GetName();
			}
			catch (...)
			{
				CachedActorName = "[Invalid Actor]";
				SelectedActor = nullptr;
			}
		}
		else
		{
			CachedActorName = "";
		}
	}

	// GizmoActor 참조 업데이트
	if (!GizmoActor && UIManager)
	{
		GizmoActor = UIManager->GetGizmoActor();
	}

	if (SelectedActor)
	{
		// 액터가 선택되어 있으면 항상 트랜스폼 정보를 업데이트하여
		// 기즈모 조작을 실시간으로 UI에 반영합니다.
		UpdateTransformFromActor();
	}
	
	// 월드 정보 업데이트 (옵션)
	if (UIManager && UIManager->GetWorld())
	{
		UWorld* World = UIManager->GetWorld();
		WorldActorCount = static_cast<uint32>(World->GetActors().size());
	}
}

void UTargetActorTransformWidget::RenderWidget()
{
	// 월드 정보 표시
	ImGui::Text("World Information");
	ImGui::Text("Actor Count: %u", WorldActorCount);
	ImGui::Separator();

	AGridActor* gridActor = UIManager->GetWorld()->GetGridActor();
	if (gridActor)
	{
		float currentLineSize = gridActor->GetLineSize();
		if (ImGui::DragFloat("Grid Spacing", &currentLineSize, 0.1f, 0.1f, 1000.0f))
		{
			gridActor->SetLineSize(currentLineSize);
			EditorINI["GridSpacing"] = std::to_string(currentLineSize);
		}
	}
	else
	{
		ImGui::Text("GridActor not found in the world.");
	}

	ImGui::Text("Transform Editor");
	
	// 기즈모 스페이스 모드 선택
	if (GizmoActor)
	{
		const char* spaceItems[] = { "World", "Local" };
		int currentSpaceIndex = static_cast<int>(CurrentGizmoSpace);
		
		if (ImGui::Combo("Gizmo Space", &currentSpaceIndex, spaceItems, IM_ARRAYSIZE(spaceItems)))
		{
			CurrentGizmoSpace = static_cast<EGizmoSpace>(currentSpaceIndex);
			
			GizmoActor->SetSpaceWorldMatrix(CurrentGizmoSpace, SelectedActor);
		}
		ImGui::Separator();
	}
	
	if (SelectedActor)
	{
		// 액터 이름 표시 (캐시된 이름 사용)
		ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "Selected: %s", 
		                   CachedActorName.c_str());
		ImGui::Spacing();
		
		// Location 편집
		if (ImGui::DragFloat3("Location", &EditLocation.X, 0.1f))
		{
			bPositionChanged = true;
		}
		
		// Rotation 편집 (Euler angles)
		if (ImGui::DragFloat3("Rotation", &EditRotation.X, 0.5f))
		{
			bRotationChanged = true;
		}
		
		// Scale 편집
		ImGui::Checkbox("Uniform Scale", &bUniformScale);
		
		if (bUniformScale)
		{
			float UniformScale = EditScale.X;
			if (ImGui::DragFloat("Scale", &UniformScale, 0.01f, 0.01f, 10.0f))
			{
				EditScale = FVector(UniformScale, UniformScale, UniformScale);
				bScaleChanged = true;
			}
		}
		else
		{
			if (ImGui::DragFloat3("Scale", &EditScale.X, 0.01f, 0.01f, 10.0f))
			{
				bScaleChanged = true;
			}
		}
		
		ImGui::Spacing();
		
		// 실시간 적용 버튼
		if (ImGui::Button("Apply Transform"))
		{
			ApplyTransformToActor();
		}
		
		ImGui::SameLine();
		if (ImGui::Button("Reset Transform"))
		{
			UpdateTransformFromActor();
			ResetChangeFlags();
		}
		
		// 기즈모 스페이스 빠른 전환 버튼
		if (GizmoActor)
		{
			ImGui::Separator();
			const char* buttonText = CurrentGizmoSpace == EGizmoSpace::World ? 
				"Switch to Local" : "Switch to World";
			
			if (ImGui::Button(buttonText))
			{
				// 스페이스 모드 전환
				CurrentGizmoSpace = (CurrentGizmoSpace == EGizmoSpace::World) ? 
					EGizmoSpace::Local : EGizmoSpace::World;
				
				// 기즈모 액터에 스페이스 설정 적용
				GizmoActor->SetSpaceWorldMatrix(CurrentGizmoSpace, SelectedActor);
			}
			
			ImGui::SameLine();
			ImGui::Text("Current: %s", 
				CurrentGizmoSpace == EGizmoSpace::World ? "World" : "Local");
		}
		
		ImGui::Spacing();
		ImGui::Separator();
	}
	else
	{
		ImGui::TextColored(ImVec4(0.7f, 0.7f, 0.7f, 1.0f), "No Actor Selected");
		ImGui::TextUnformatted("Select an actor to edit its transform.");
	}
	
	ImGui::Separator();
}

void UTargetActorTransformWidget::PostProcess()
{
	// 자동 적용이 활성화된 경우 변경사항을 즉시 적용
	if (bPositionChanged || bRotationChanged || bScaleChanged)
	{
		ApplyTransformToActor();
		ResetChangeFlags(); // 적용 후 플래그 리셋
	}
}

void UTargetActorTransformWidget::UpdateTransformFromActor()
{
	if (!SelectedActor)
		return;
		
	// 액터의 현재 트랜스폼을 UI 변수로 복사
	EditLocation = SelectedActor->GetActorLocation();
	EditRotation = SelectedActor->GetActorRotation().ToEuler();
	EditScale = SelectedActor->GetActorScale();
	
	// 균등 스케일 여부 판단
	bUniformScale = (abs(EditScale.X - EditScale.Y) < 0.01f && 
	                abs(EditScale.Y - EditScale.Z) < 0.01f);
	
	ResetChangeFlags();
}

void UTargetActorTransformWidget::ApplyTransformToActor() const
{
	if (!SelectedActor)
		return;
		
	// 변경사항이 있는 경우에만 적용
	if (bPositionChanged)
	{
		SelectedActor->SetActorLocation(EditLocation);
		UE_LOG("Transform: Applied location (%.2f, %.2f, %.2f)", 
		       EditLocation.X, EditLocation.Y, EditLocation.Z);
	}
	
	if (bRotationChanged)
	{
		FQuat NewRotation = FQuat::MakeFromEuler(EditRotation);
		SelectedActor->SetActorRotation(NewRotation);
		UE_LOG("Transform: Applied rotation (%.1f, %.1f, %.1f)", 
		       EditRotation.X, EditRotation.Y, EditRotation.Z);
	}
	
	if (bScaleChanged)
	{
		SelectedActor->SetActorScale(EditScale);
		UE_LOG("Transform: Applied scale (%.2f, %.2f, %.2f)", 
		       EditScale.X, EditScale.Y, EditScale.Z);
	}
	
	// 플래그 리셋은 const 메서드에서 할 수 없으므로 PostProcess에서 처리
}

void UTargetActorTransformWidget::ResetChangeFlags()
{
	bPositionChanged = false;
	bRotationChanged = false;
	bScaleChanged = false;
}
