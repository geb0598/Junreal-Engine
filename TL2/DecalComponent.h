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

    void TickComponent(float DeltaSeconds) override;
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

    // Fade Effect Getter/Setter Func
    int32 GetSortOrder() const { return SortOrder; }
    float GetFadeScreenSize() const { return FadeScreenSize; }
    float GetFadeStartDelay() const { return FadeStartDelay; }
    float GetFadeDuration() const { return FadeDuration; }
    float GetFadeInStartDelay() const { return FadeInStartDelay; }
    float GetFadeInDuration() const { return FadeInDuration; }
    
    void SetSortOrder(int32 InSortOrder) { SortOrder = InSortOrder; }
    void SetFadeScreenSize(float InFadeScreenSize) { FadeScreenSize = InFadeScreenSize; }
    void SetFadeStartDelay (float InFadeStartDelay ) { FadeStartDelay = InFadeStartDelay; }
    void SetFadeDuration (float InFadeDuration ) { FadeDuration = InFadeDuration; }
    void SetFadeInStartDelay (float InFadeInStartDelay ) { FadeInStartDelay = InFadeInStartDelay; }
    void SetFadeInDuration(float InFadeInDuration) { FadeInDuration = InFadeInDuration; }

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

private:
    // Decal fade state
    enum class EFadeState
    {
        None,
        FadingIn,
        FadingOut
    };

    EFadeState CurrentFadeState = EFadeState::None;

    float CurrentAlpha = 1.0f;
    float LifetimeTimer = 0.0f;
    
    int32 SortOrder = 0;
    float FadeScreenSize = 0.01f;
    float FadeStartDelay = 30.0f;
    float FadeDuration = 10.0f;
    float FadeInStartDelay = 10.0f;
    float FadeInDuration = 10.0f;

    bool bIsDirty = true;
};