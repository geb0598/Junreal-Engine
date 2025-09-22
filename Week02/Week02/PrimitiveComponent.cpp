#include "pch.h"
#include "PrimitiveComponent.h"

void UPrimitiveComponent::SetMaterial(const FString& FilePath, EVertexLayoutType layoutType)
{
    //Material = UResourceManager::GetInstance().GetOrCreateMaterial(FilePath, layoutType);
    /*if (FilePath == "StaticMeshShader.hlsl")
    {
        UE_LOG("");
    }*/
    Material = UResourceManager::GetInstance().Load<UMaterial>(FilePath, layoutType);
}