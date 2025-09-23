#include "pch.h"
#include "PrimitiveComponent.h"
#include "SceneLoader.h"
#include "SceneComponent.h"

void UPrimitiveComponent::SetMaterial(const FString& FilePath, EVertexLayoutType layoutType)
{
    Material = UResourceManager::GetInstance().Load<UMaterial>(FilePath, layoutType);
}

void UPrimitiveComponent::Serialize(bool bIsLoading, FPrimitiveData& InOut)
{
    if (bIsLoading)
    {
        // FPrimitiveData -> 컴포넌트 월드 트랜스폼
        FTransform WT;
        WT.Translation = InOut.Location;
        WT.Rotation = FQuat::MakeFromEuler(InOut.Rotation);
        WT.Scale3D = InOut.Scale;
        SetWorldTransform(WT);
    }
    else
    {
        // 컴포넌트 월드 트랜스폼 -> FPrimitiveData
        const FTransform WT = GetWorldTransform();
        InOut.Location = WT.Translation;
        InOut.Rotation = WT.Rotation.ToEuler();
        InOut.Scale = WT.Scale3D;
    }
}