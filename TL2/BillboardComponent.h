#pragma once

#pragma once
#include "MeshComponent.h"
class UBillboardComponent : public UMeshComponent
{
public:
    DECLARE_CLASS(UBillboardComponent, UMeshComponent)
    UBillboardComponent();

protected:
    ~UBillboardComponent() override;

public:
    void InitCharInfoMap();
    TArray<FBillboardVertexInfo_GPU> CreateVerticesForString(const FString& text,const FVector& StartPos);
    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;

    UTextQuad* GetStaticMesh() const { return TextQuad; }

private:
    FString Text;
    TMap<char, FBillboardVertexInfo> CharInfoMap;
    FString TextureFilePath;

    // TODO: UStaticMesh는 UStaticMeshComponent만 사용하도록 바꿔야 한다
    UTextQuad* TextQuad = nullptr;
};