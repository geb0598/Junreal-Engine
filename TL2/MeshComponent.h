#pragma once
#include "PrimitiveComponent.h"
#include "BoundingVolume.h"

class UMeshComponent : public UPrimitiveComponent
{
public:
    DECLARE_CLASS(UMeshComponent, UPrimitiveComponent)
    UMeshComponent();

protected:
    ~UMeshComponent() override;

public:


protected:

};