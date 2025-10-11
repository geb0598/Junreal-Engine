// DecalComponent.h
#pragma once
#include "PrimitiveComponent.h"
#include "StaticMesh.h"

class UDecalComponent : public UPrimitiveComponent
{
public:
    DECLARE_CLASS(UDecalComponent, UPrimitiveComponent)

    UDecalComponent();
    virtual ~UDecalComponent() override;

    void Render(URenderer* Renderer, UPrimitiveComponent* Component, const FMatrix& View, const FMatrix& Proj, FViewport* Viewport) ;

    // 데칼 크기 설정 (박스 볼륨의 크기)
    void SetDecalSize(const FVector& InSize);
    FVector GetDecalSize() const { return DecalSize; }

    // UV 타일링 설정
    void SetUVTiling(const FVector2D& InTiling) { UVTiling = InTiling; UpdateDecalProjectionMatrix(); }
    FVector2D GetUVTiling() const { return UVTiling; }

    // 데칼 텍스처 설정
    void SetDecalTexture(const FString& TexturePath);

    UObject* Duplicate() override;
    void DuplicateSubObjects() override;

    UStaticMesh* GetDecalBoxMesh() const { return DecalBoxMesh; }
protected:

    void UpdateDecalProjectionMatrix();

    // 데칼 박스 메쉬 (큐브)
    UStaticMesh* DecalBoxMesh = nullptr;
    FMatrix DecalProjectionMatrix;

    // 데칼 크기
    FVector DecalSize = FVector(1.0f, 1.0f, 1.0f);

    // UV 타일링
    FVector2D UVTiling = FVector2D(4.0f, 4.0f);

    // 데칼 블렌드 모드
    enum class EDecalBlendMode
    {
        Translucent,    // 반투명 블렌딩
        Stain,          // 곱셈 블렌딩
        Normal,         // 노멀맵
        Emissive        // 발광
    };

    EDecalBlendMode BlendMode = EDecalBlendMode::Translucent;
};
