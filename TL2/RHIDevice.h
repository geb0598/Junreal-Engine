#pragma once
#include <d3d11.h>
#include <d3dcompiler.h>
#include "Vector.h"
#include "CBufferTypes.h"

#define UPDATE_CBUFFER_VIRTUAL_FUNC(TYPE)\
    virtual void UpdateCBuffer(const TYPE& CBufferData) = 0;
#define UPDATE_SET_CBUFFER_VIRTUAL_FUNC(TYPE)\
    virtual void UpdateSetCBuffer(const TYPE& CBufferData) = 0;
#define SET_CBUFFER_VIRTUAL_FUNC(TYPE)\
    virtual void SetCBuffer(const TYPE& CBufferData) = 0;


enum class EComparisonFunc
{
    Always,
    LessEqual,
    GreaterEqual,
    // 필요하면 추가
      // 필요시 추가
    Disable,
    LessEqualReadOnly,
};
class URHIDevice
{
public:
    URHIDevice() {};
    virtual ~URHIDevice() {};

private:
    // 복사생성자, 연산자 금지
    URHIDevice(const URHIDevice& RHIDevice) = delete;
    URHIDevice& operator=(const URHIDevice& RHIDevice) = delete;

public:
    virtual void Initialize(HWND hWindow) = 0;
    virtual void Release() = 0;
public:
    //getter
    virtual ID3D11Device* GetDevice() = 0;
    virtual ID3D11DeviceContext* GetDeviceContext() = 0;
    virtual IDXGISwapChain* GetSwapChain() = 0;

    // create
    virtual void CreateDeviceAndSwapChain(HWND hWindow) = 0;;
    virtual void CreateFrameBuffer() = 0;
    virtual void CreateRasterizerState() = 0;
    virtual void CreateConstantBuffer() = 0;
    virtual void CreateBlendState() = 0;
	virtual void CreateDepthStencilState() = 0;
    virtual void CreateShader(ID3D11InputLayout** OutSimpleInputLayout, ID3D11VertexShader** OutSimpleVertexShader, ID3D11PixelShader** OutSimplePixelShader) = 0;

    // update
    CBUFFER_TYPE_LIST(UPDATE_CBUFFER_VIRTUAL_FUNC)
    CBUFFER_TYPE_LIST(UPDATE_SET_CBUFFER_VIRTUAL_FUNC)
    CBUFFER_TYPE_LIST(SET_CBUFFER_VIRTUAL_FUNC)
    virtual void UpdateConstantBuffers(const ModelBufferType& ModelConstant, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix) = 0;
    virtual void UpdateBillboardConstantBuffers(const FVector& pos, const FMatrix& ViewMatrix, const FMatrix& ProjMatrix, const FVector& CameraRight, const FVector& CameraUp)=0;
    virtual void UpdatePixelConstantBuffers(const FObjMaterialInfo& InMaterialInfo, bool bHasMaterial, bool bHasTexture) = 0;
    virtual void UpdateHighLightConstantBuffers(const uint32 InPicked, const FVector& InColor, const uint32 X, const uint32 Y, const uint32 Z, const uint32 Gizmo) = 0;
    virtual void UpdateColorConstantBuffers(const FVector4& InColor) = 0;
    virtual void UpdateUVScrollConstantBuffers(const FVector2D& Speed, float TimeSec) = 0;
    virtual void UpdateInvWorldConstantBuffer(const FMatrix& DecalWorldMatrix, const FMatrix& DecalWorldMatrixInverse, const FMatrix& DecalProjectionMatrix) = 0;
    virtual void UpdateDecalConstantBuffer(float InFadeAlpha) = 0;

    // clear
    virtual void ClearBackBuffer() = 0;
    virtual void ClearDepthBuffer(float Depth, UINT Stenci) = 0;

    virtual void IASetPrimitiveTopology() = 0;
    virtual void RSSetViewport() = 0;
    virtual void RSSetState(EViewModeIndex ViewModeIndex) = 0;
    virtual void RSSetFrontCullState() = 0;
    virtual void RSSetNoCullState() = 0;
    virtual void RSSetDefaultState() = 0;
    virtual void OMSetRenderTargets() = 0;
    virtual void OMSetBlendState(bool bIsBlendMode) = 0;
    virtual void OmSetDepthStencilState(EComparisonFunc Func) = 0;
    virtual void Present() = 0;
    virtual void PSSetDefaultSampler(UINT StartSlot) = 0;
};

