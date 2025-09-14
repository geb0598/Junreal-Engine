#pragma once
#include "RHIDevice.h"
#include "VertexSimple.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class URHIDevice;
class UShader;
class UMesh;

class URenderer
{
public:
    URenderer(URHIDevice* InDevice);

    ~URenderer();

public:
	void BeginFrame();

    //void PrepareShader();

    void PrepareShader(FShader& InShader);

    void PrepareShader(UShader* InShader);

    void OMSetBlendState(bool bIsChecked);

    void UpdateConstantBuffer(const FMatrix& ModelMatrix, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix);

    void UpdateHighLightConstantBuffer(const float InPicked, const FVector& InColor, const uint32 X, const uint32 Y, const uint32 Z, const uint32 Gizmo);

    void UpdateBillboardConstantBuffers(const FMatrix& ViewMatrix, const FMatrix& ProjMatrix, const FVector& CameraRight, const FVector& CameraUp);

    //void DrawPrimitiveComponent(UStaticMeshComponent* MeshComp);
    
    void DrawIndexedPrimitiveComponent(UStaticMeshComponent* MeshComp);

    void DrawIndexedPrimitiveComponent(UMesh* MeshComp, D3D11_PRIMITIVE_TOPOLOGY InTopology);

    void DrawIndexedPrimitiveComponent(UTextRenderComponent* TextRenderComp);

	void EndFrame();

    URHIDevice*&const  GetRHIDevice() { return RHIDevice; }
private:
	URHIDevice* RHIDevice;
};

