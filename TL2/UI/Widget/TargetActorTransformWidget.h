﻿#pragma once
#include "Widget.h"
#include "../../Vector.h"
#include "../../Enums.h"

class UUIManager;
class UWorld;
class AActor;
class AGizmoActor;

class UTargetActorTransformWidget
	: public UWidget
{
public:
	DECLARE_CLASS(UTargetActorTransformWidget, UWidget)

	// Special Member Function
	UTargetActorTransformWidget();
	~UTargetActorTransformWidget() override;

	void Initialize() override;
	void Update() override;
	void DuplicateTarget(AActor* SelectedActor) const;
	void RenderWidget() override;
	void PostProcess() override;

	void UpdateTransformFromActor();
	void ApplyTransformToComponent(USceneComponent* SelectedComponent) const;

	void RenderComponentHierarchy(USceneComponent* SceneComponent) ;

	// 선택된 액터가 외부에서 삭제되었을 때 호출되어 내부 상태를 정리
	void OnSelectedActorCleared();

private:
	UUIManager* UIManager = nullptr;
	USelectionManager* SelectionManager = nullptr;
	//AActor* SelectedActor = nullptr;
	//USceneComponent* SelectedComponent = nullptr;	// 현재 선택된 SceneComponent를 저장할 포인터
	FString CachedActorName; // 액터 이름 캐시 (안전한 출력을 위해)

	// Transform UI 상태
	FVector EditLocation = {0.0f, 0.0f, 0.0f};
	FVector EditRotation = {0.0f, 0.0f, 0.0f};
	FVector EditScale = {1.0f, 1.0f, 1.0f};
	
	// UI 변경 플래그
	bool bScaleChanged = false;
	bool bRotationChanged = false;
	bool bPositionChanged = false;
	bool bUniformScale = false;
	
	// 헬퍼 메서드
	AActor* GetCurrentSelectedActor() const;
	void ResetChangeFlags();
};
