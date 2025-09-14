#pragma once
#include "SceneComponent.h"
#include "Material.h"

// 렌더링/콜리전 속성
// 여기는 보여질 것인지 ? 메쉬 콜리전 사용할 것인지 ? 
// 속성 종류 
class UPrimitiveComponent :public USceneComponent
{
public:
    DECLARE_CLASS(UPrimitiveComponent, USceneComponent)

    UPrimitiveComponent();

protected:
    ~UPrimitiveComponent() override;

public:

};

