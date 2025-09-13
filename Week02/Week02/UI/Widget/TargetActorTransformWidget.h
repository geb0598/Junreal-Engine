#pragma once
#include "Widget.h"
#include "../../Vector.h"

class UUIManager;
class UWorld;
class AActor;

class UTargetActorTransformWidget
	: public UWidget
{
public:
	DECLARE_CLASS(UTargetActorTransformWidget, UWidget)

	void Initialize() override;
	void Update() override;
	void RenderWidget() override;
	void PostProcess() override;

	void UpdateTransformFromActor();
	void ApplyTransformToActor() const;

	// Special Member Function
	UTargetActorTransformWidget();
	~UTargetActorTransformWidget() override;

private:
	UUIManager* UIManager = nullptr;
	AActor* SelectedActor = nullptr;

	// Transform UI 상태
	FVector EditLocation = {0.0f, 0.0f, 0.0f};
	FVector EditRotation = {0.0f, 0.0f, 0.0f};
	FVector EditScale = {1.0f, 1.0f, 1.0f};
	
	// UI 변경 플래그
	bool bScaleChanged = false;
	bool bRotationChanged = false;
	bool bPositionChanged = false;
	bool bUniformScale = true;
	
	// 월드 정보 (옵션)
	uint32 WorldActorCount = 0;
	
	// 헬퍼 메서드
	AActor* GetCurrentSelectedActor() const;
	void ResetChangeFlags();
};
