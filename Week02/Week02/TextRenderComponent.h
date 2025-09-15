#pragma once
#include "MeshComponent.h"
class UTextRenderComponent : public UMeshComponent
{
public:
	DECLARE_CLASS(UTextRenderComponent, UMeshComponent)
	UTextRenderComponent();

protected:
	~UTextRenderComponent() override;

public:
	void InitCharInfoMap();
	TArray<FBillboardVertexInfo_GPU> CreateVerticesForString(const FString& text,const FVector& StartPos);
	//FResourceData* GetResourceData() { return ResourceData; }
	//FTextureData* GetTextureData() { return TextureData; }
	virtual void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
	void SetText(FString Txt);
private:
	FString Text;
	TMap<char, FBillboardVertexInfo> CharInfoMap;
	FString TextureFilePath;
};