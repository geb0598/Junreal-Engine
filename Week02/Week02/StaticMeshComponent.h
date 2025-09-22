#pragma once
#include "MeshComponent.h"
#include "Enums.h"
#include "StaticMesh.h"

class UStaticMesh;
class UShader;
class UTexture;
struct FPrimitiveData;

class UStaticMeshComponent : public UMeshComponent
{
public:
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)
    UStaticMeshComponent();

protected:
    ~UStaticMeshComponent() override;

public:
    void Render(URenderer* Renderer, const FMatrix& View, const FMatrix& Proj) override;

    void SetStaticMesh(const FString& PathFileName);
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }

    // 씬 포맷(FPrimitiveData)을 이용한 컴포넌트 직렬화/역직렬화
    // - bIsLoading == true  : InOut로부터 읽어서 컴포넌트 상태(메시) 설정
    // - bIsLoading == false : 컴포넌트 상태를 InOut에 기록
    void Serialize(bool bIsLoading, FPrimitiveData& InOut);
    
protected:
    UStaticMesh* StaticMesh = nullptr;
};

