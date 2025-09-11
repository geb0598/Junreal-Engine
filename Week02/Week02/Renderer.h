#pragma once
#include "RHIDevice.h"
#include "Vector.h"
#include "VertexSimple.h"
#include "StaticMeshComponent.h"

class URHIDevice;
class URenderer
{
public:
    URenderer(URHIDevice* InDevice);

    ~URenderer();

public:
	void BeginFrame();

    //void PrepareShader();

    void PrepareShader(FShader& InShader);

    void OMSetBlendState(bool bIsChecked);

    void UpdateConstantBuffer(const FMatrix& ModelMatrix, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix);

    void UpdateHighLightConstantBuffer(const float InPicked, const FVector& InColor, const uint32 X, const uint32 Y, const uint32 Z, const uint32 Gizmo);

    //void DrawPrimitiveComponent(UStaticMeshComponent* MeshComp);
    
    void DrawIndexedPrimitiveComponent(UStaticMeshComponent* MeshComp);

	void EndFrame();

    URHIDevice*&const  GetRHIDevice() { return RHIDevice; }
private:
	URHIDevice* RHIDevice;
};

