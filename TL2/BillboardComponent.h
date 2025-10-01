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
    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;
    void SetBillboardSize(float Width, float Height) { BillboardWidth = Width; BillboardHeight = Height; }
    void SetTexture(const FString& TexturePath);

    UTextQuad* GetStaticMesh() const { return BillboardQuad; }

private:
    void CreateBillboardVertices();

private:
    UTextQuad* BillboardQuad = nullptr;
    float BillboardWidth = 1.0f;
    float BillboardHeight = 1.0f;
    FString TexturePath = "Editor/Icon/Pawn_64x.dds";
};