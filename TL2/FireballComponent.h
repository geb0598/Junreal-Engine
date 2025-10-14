#pragma once
#include"PrimitiveComponent.h"
#include"LinearColor.h"
class UFireBallComponent : public UPrimitiveComponent
{
public:
	DECLARE_CLASS(UFireBallComponent, UPrimitiveComponent)
	UFireBallComponent();
    ~UFireBallComponent() override;
    // 🔹 FireBall의 물리적/시각적 속성
   

    // 🔹 Render 함수 (Renderer에서 호출)
	virtual void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj, const EEngineShowFlags ShowFlags) override;

    // 🔹 Update 함수 (필요시 시간 기반 변화)
    virtual void TickComponent(float DeltaSeconds) override;

    // 🔹 AABB 반환 (충돌/선택 처리용)
    virtual const FAABB GetWorldAABB() const override;


    float Intensity = 5.0f;          // 밝기 (빛 세기)
    float Radius = 15.0f;           // 영향 반경
    float RadiusFallOff = 2.0f;      // 감쇠 정도 (클수록 급격히 사라짐)
    FLinearColor Color = FLinearColor(1.f, 0.0f, 0.0f, 1.f); // 오렌지빛

    // 🔸 CPU → GPU 전달용 라이트 데이터 캐시
    FPointLightData PointLightBuffer;

protected:
	

    UObject* Duplicate() override;
    void DuplicateSubObjects() override;
};
