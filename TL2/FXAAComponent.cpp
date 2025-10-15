#include "pch.h"
#include "FXAAComponent.h"

void UFXAAComponent::Render(URenderer* Renderer)
{
	Renderer->UpdateSetCBuffer(FXAABufferType(SlideX, SpanMax, (ReduceMin / 128.0f), ReduceMul));
	Renderer->RenderPostProcessing(UResourceManager::GetInstance().Load<UShader>("FXAA.hlsl"));
}

UObject* UFXAAComponent::Duplicate()
{
	UFXAAComponent* DuplicatedComponent = Cast<UFXAAComponent>(NewObject(GetClass()));
	if (DuplicatedComponent)
	{
		CopyCommonProperties(DuplicatedComponent);
		DuplicatedComponent->SlideX = SlideX;
		DuplicatedComponent->SpanMax = SpanMax;
		DuplicatedComponent->ReduceMin = ReduceMin;
		DuplicatedComponent->ReduceMul = ReduceMul;
	}
	DuplicatedComponent->DuplicateSubObjects();

	return DuplicatedComponent;
}

void UFXAAComponent::DuplicateSubObjects()
{
	USceneComponent::DuplicateSubObjects();
}