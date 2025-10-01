#pragma once
#include "MeshComponent.h"
class UTextRenderComponent : public UPrimitiveComponent
{
public:
	DECLARE_CLASS(UTextRenderComponent, UMeshComponent)
	UTextRenderComponent();

protected:
	~UTextRenderComponent() override;

public:
	void InitCharInfoMap();
	TArray<FBillboardVertexInfo_GPU> CreateVerticesForString(const FString& text,const FVector& StartPos);
	virtual void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
	
	void SetText(FString InText);

	UTextQuad* GetStaticMesh() const { return TextQuad; }

private:
	FString Text;
	TMap<char, FBillboardVertexInfo> CharInfoMap;

	UTextQuad* TextQuad = nullptr;

	bool bIsDirty = true;	// Text 가 변경된 경우 true 후 TextQuad 업데이트
};