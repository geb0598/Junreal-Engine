#include"pch.h"
#include "FireBallComponent.h"
#include "Renderer.h"
#include "World.h"



UFireBallComponent::UFireBallComponent()
{
    // 초기 라이트 데이터 세팅
    FVector WorldPos = GetWorldLocation();
    PointLightBuffer.Position = FVector4(WorldPos, Radius);
    PointLightBuffer.Color = FVector4(Color.R, Color.G, Color.B, Intensity);
    PointLightBuffer.FallOff = RadiusFallOff;

   
}

UFireBallComponent::~UFireBallComponent()
{
}

void UFireBallComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj, const EEngineShowFlags ShowFlags)
{
  
}

void UFireBallComponent::TickComponent(float DeltaSeconds)
{

    Radius = 300.0f + sinf(GetWorld()->GetTimeSeconds() * 3.0f) * 50.0f;

    // 🔹 GPU 업로드용 버퍼 갱신
    FVector WorldPos = GetWorldLocation();
    PointLightBuffer.Position = FVector4(WorldPos, Radius);
    PointLightBuffer.Color = FVector4(Color.R, Color.G, Color.B, Intensity);
    PointLightBuffer.FallOff = RadiusFallOff;
}

const FAABB UFireBallComponent::GetWorldAABB() const
{
    // FireBall의 Radius를 기반으로 AABB 생성
    FVector WorldLocation = GetWorldLocation();
    FVector Extent(Radius, Radius, Radius);

    return FAABB(WorldLocation - Extent, WorldLocation + Extent);
}

UObject* UFireBallComponent::Duplicate()
{
    UFireBallComponent* NewComp = NewObject<UFireBallComponent>();
    *NewComp = *this; // 복제 (단, UObject 포인터 복사는 주의)
    NewComp->DuplicateSubObjects();
    return NewComp;
}

void UFireBallComponent::DuplicateSubObjects()
{
	Super_t::DuplicateSubObjects();
}


