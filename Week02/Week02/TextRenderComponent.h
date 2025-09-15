#pragma once
#include "MeshComponent.h"
#include "CharacterInfo.h"
class UTextRenderComponent : public UMeshComponent
{
public:
	DECLARE_CLASS(UTextRenderComponent, UMeshComponent)
	UTextRenderComponent();
	~UTextRenderComponent() override;

protected:

public:
	void InitCharInfoMap();
	TArray<FBillboardCharInfo> CreateVerticesForString(const FString& text,const FVector& StartPos);
	FResourceData* GetResourceData() { return ResourceData; }
	FTextureData* GetTextureData() { return TextureData; }
	virtual void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
	void SetText(FString Txt);
private:
	FString Text;
	TMap<char, FCharacterInfo> CharInfoMap;
	FString TextureFilePath;
	FResourceData* ResourceData = nullptr;
	FTextureData* TextureData = nullptr;
};