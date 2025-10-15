#pragma once
#include"PrimitiveComponent.h"
#include"LinearColor.h"

struct FFireBallProperty
{
    float Intensity = 5.0f;           // 밝기 (빛 세기)
    float Radius = 15.0f;             // 영향 반경
    float RadiusFallOff = 2.0f;       // 감쇠 정도 (클수록 급격히 사라짐)
    FLinearColor Color = FLinearColor(1.f, 0.0f, 0.0f, 1.f); // 오렌지빛
};

class UFireBallComponent : public UPrimitiveComponent
{
public:
	DECLARE_CLASS(UFireBallComponent, UPrimitiveComponent)
	UFireBallComponent();
    ~UFireBallComponent() override;
    // 🔹 FireBall의 물리적/시각적 속성
   
    virtual void Serialize(bool bIsLoading, FComponentData& InOut) override;
    // 🔹 Render 함수 (Renderer에서 호출)
	virtual void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj, const EEngineShowFlags ShowFlags) override;

    // 🔹 Update 함수 (필요시 시간 기반 변화)
    virtual void TickComponent(float DeltaSeconds) override;

    // 🔹 AABB 반환 (충돌/선택 처리용)
    virtual const FAABB GetWorldAABB() const override;

    FFireBallProperty FireData;
 

    // 🔸 CPU → GPU 전달용 라이트 데이터 캐시
    FPointLightData PointLightBuffer;

protected:
	

    UObject* Duplicate() override;
    void DuplicateSubObjects() override;
};
