#include "pch.h"
#include "SpotLightActor.h"
//#include "ObjectFactory.h"
//
//ASpotLightActor::ASpotLightActor()
//{
//    SpotLightComponent = CreateDefaultSubobject<USpotLightComponent>(FName("SpotLightComponent"));
//    RootComponent = SpotLightComponent;
//
//    SpriteComponent = CreateDefaultSubobject<UBillboardComponent>(FName("SpriteComponent"));
//    if (SpriteComponent)
//    {
//        SpriteComponent->SetTexture(FString("Editor/Icon/SpotLight_64x.dds"));
//        SpriteComponent->SetRelativeLocation(RootComponent->GetWorldLocation());
//        SpriteComponent->SetEditable(false);
//        SpriteComponent->SetupAttachment(RootComponent);
//    }
//}
//
//ASpotLightActor::~ASpotLightActor()
//{
//}
//void ASpotLightActor::Tick(float DeltaTime)
//{
//    SpriteComponent->SetTexture(FString("Editor/Icon/SpotLight_64x.dds"));
//}
//
//UObject* ASpotLightActor::Duplicate()
//{
//    // 부모 클래스의 Duplicate 호출 (RootComponent와 모든 자식 컴포넌트 복제)
//    ASpotLightActor* NewActor = static_cast<ASpotLightActor*>(AActor::Duplicate());
//    return NewActor;
//}
//
//void ASpotLightActor::DuplicateSubObjects()
//{
//    // 부모 클래스가 OwnedComponents를 재구성
//    AActor::DuplicateSubObjects();
//
//    // OwnedComponents를 순회하면서 각 타입의 컴포넌트를 찾아 포인터 재설정
//    for (UActorComponent* Component : OwnedComponents)
//    {
//        if (USpotLightComponent* SpotLight = Cast<USpotLightComponent>(Component))
//        {
//            SpotLightComponent = SpotLight;
//        }
//        else if (UBillboardComponent* Billboard = Cast<UBillboardComponent>(Component))
//        {
//            SpriteComponent = Billboard;
//        }
//    }
//}
