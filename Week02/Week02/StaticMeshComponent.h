#pragma once
#include "MeshComponent.h"
#include "Enums.h"
#include "StaticMesh.h"

class UStaticMeshComponent : public UMeshComponent
{
public:
    DECLARE_CLASS(UStaticMeshComponent, UMeshComponent)
    UStaticMeshComponent();

protected:
    ~UStaticMeshComponent() override;

public:
    UStaticMesh* GetStaticMesh() const { return StaticMesh; }
    void SetStaticMesh(const FString& FilePath);
    void SetStaticMesh(UStaticMesh* InStaticMesh) { StaticMesh = InStaticMesh; }

protected:
    //FString FilePath;
    //ResourceData* StaticMeshData = nullptr;

    UStaticMesh* StaticMesh;
};

