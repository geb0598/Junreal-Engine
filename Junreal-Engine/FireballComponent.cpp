#include"pch.h"
#include "FireBallComponent.h"
#include "Renderer.h"
#include "World.h"
#include"SceneLoader.h"


UFireBallComponent::UFireBallComponent()
{
    // 초기 라이트 데이터 세팅
    FVector WorldPos = GetWorldLocation();
    PointLightBuffer.Position = FVector4(WorldPos, FireData.Radius);
    PointLightBuffer.Color = FVector4(FireData.Color.R, FireData.Color.G, FireData.Color.B, FireData.Intensity);
    PointLightBuffer.FallOff = FireData.RadiusFallOff;

    bCanEverTick = true;
}

UFireBallComponent::~UFireBallComponent()
{
}

void UFireBallComponent::Serialize(bool bIsLoading, FComponentData& InOut)
{
    UPrimitiveComponent::Serialize(bIsLoading, InOut);

    if (bIsLoading)
    {
        FireData = InOut.FireBallProperty;
    }
    else
    {
        InOut.FireBallProperty = FireData;
    }
}

void UFireBallComponent::Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj, const EEngineShowFlags ShowFlags)
{
  
}

void UFireBallComponent::TickComponent(float DeltaSeconds)
{
    static int Time;
    Time += DeltaSeconds;
   // Radius = 300.0f + sinf(Time) * 100.0f;

    //// 🔹 GPU 업로드용 버퍼 갱신
    //FVector WorldPos = GetWorldLocation();
    //PointLightBuffer.Position = FVector4(WorldPos, FireData.Radius);
    //PointLightBuffer.Color = FVector4(FireData.Color.R, FireData.Color.G, FireData.Color.B, FireData.Intensity);
    //PointLightBuffer.FallOff = FireData.RadiusFallOff;
}

const FAABB UFireBallComponent::GetWorldAABB() const
{
    // FireBall의 Radius를 기반으로 AABB 생성
    FVector WorldLocation = GetWorldLocation();
    FVector Extent(FireData.Radius, FireData.Radius, FireData.Radius);

    return FAABB(WorldLocation - Extent, WorldLocation + Extent);
}

UObject* UFireBallComponent::Duplicate()
{
    UFireBallComponent* DuplicatedComponent = NewObject<UFireBallComponent>();
    CopyCommonProperties(DuplicatedComponent);
    DuplicatedComponent->FireData =this->FireData ; // 복제 (단, UObject 포인터 복사는 주의)
    DuplicatedComponent->DuplicateSubObjects();
    return DuplicatedComponent;
}

void UFireBallComponent::DuplicateSubObjects()
{
	Super_t::DuplicateSubObjects();
}


