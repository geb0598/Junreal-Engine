#pragma once
#include "RHIDevice.h"
#include "ResourceManager.h"

#define DECLARE_CBUFFER(TYPE)\
    ID3D11Buffer* TYPE##uffer{};

#define DECLARE_CBUFFER_UPDATE_FUNC(TYPE)\
    void UpdateCBuffer(const TYPE& CBufferData) override \
{\
    CBufferUpdate(TYPE##uffer, CBufferData);\
}

#define DECLARE_CBUFFER_UPDATE_SET_FUNC(TYPE)\
    void UpdateSetCBuffer(const TYPE& CBufferData) override\
{\
    CBufferUpdateSet(TYPE##uffer, CBufferData, TYPE##SlotNum, TYPE##SetVS, TYPE##SetPS);\
}

#define DECLARE_CBUFFER_SET_FUNC(TYPE)\
    void SetCBuffer(const TYPE& CBufferData) override\
{\
    CBufferSet(TYPE##uffer, TYPE##SlotNum, TYPE##SetVS, TYPE##SetPS);\
}

#define CREATE_CBUFFER(TYPE)\
CreateCBuffer(&TYPE##uffer, sizeof(TYPE));

#define RELEASE_CBUFFER(TYPE)\
if(TYPE##uffer) {TYPE##uffer->Release(); TYPE##uffer = nullptr;}


class D3D11RHI : public URHIDevice
{
public:
    D3D11RHI() {};
    ~D3D11RHI()
    {
        Release();
    }



public:
    void Initialize(HWND hWindow) override;

    void Release()override;


public:
    // clear
    void ClearBackBuffer() override;
    void ClearDepthBuffer(float Depth, UINT Stencil) override;
    void CreateBlendState() override;

    template<typename TVertex>
    static HRESULT CreateVertexBufferImpl(ID3D11Device* device, const FMeshData& mesh, ID3D11Buffer** outBuffer, D3D11_USAGE usage, UINT cpuAccessFlags);

    template<typename TVeretex>
    static HRESULT CreateVertexBuffer(ID3D11Device* device, const FMeshData& mesh, ID3D11Buffer** outBuffer);

    template<typename TVertex>
    static HRESULT CreateVertexBufferImpl(ID3D11Device* device, const std::vector<FNormalVertex>& srcVertices, ID3D11Buffer** outBuffer, D3D11_USAGE usage, UINT cpuAccessFlags);

    template<typename TVertex>
    static HRESULT CreateVertexBuffer(ID3D11Device* device, const std::vector<FNormalVertex>& srcVertices, ID3D11Buffer** outBuffer);

    static HRESULT CreateIndexBuffer(ID3D11Device* device, const FMeshData* meshData, ID3D11Buffer** outBuffer);

    static HRESULT CreateIndexBuffer(ID3D11Device* device, const FStaticMesh* mesh, ID3D11Buffer** outBuffer);

    CBUFFER_TYPE_LIST(DECLARE_CBUFFER_UPDATE_FUNC)
        CBUFFER_TYPE_LIST(DECLARE_CBUFFER_UPDATE_SET_FUNC)
        CBUFFER_TYPE_LIST(DECLARE_CBUFFER_SET_FUNC)

    void IASetPrimitiveTopology() override;
    void RSSetState(EViewModeIndex ViewModeIndex) override;
    void RSSetFrontCullState() override;
    void RSSetNoCullState() override;
    void RSSetDefaultState() override;
    void RSSetDecalState() override;
    void RSSetViewport() override;
    void OMSetRenderTargets() override;
    void OMSetBlendState(bool bIsBlendMode) override;
    void Present() override;
	void PSSetDefaultSampler(UINT StartSlot) override;

    void CreateShader(ID3D11InputLayout** OutSimpleInputLayout, ID3D11VertexShader** OutSimpleVertexShader, ID3D11PixelShader** OutSimplePixelShader) override;

    void CreateBackBufferAndDepthStencil(UINT width, UINT height);

    void SetViewport(UINT width, UINT height);

    void setviewort(UINT width, UINT height);

    void ResizeSwapChain(UINT width, UINT height);

public:
    // getter
    inline ID3D11Device* GetDevice()
    {
        return Device;
    }
    inline ID3D11DeviceContext* GetDeviceContext()
    {
        return DeviceContext;
    }
    inline IDXGISwapChain* GetSwapChain()
    {
        return SwapChain;
    }
    inline ID3D11ShaderResourceView* GetDepthSRV()
    {
        return DepthSRV;
    }
    inline ID3D11Texture2D* GetIdBuffer()
    {
        return IdBuffer;
    }
    inline ID3D11Texture2D* GetIdStagingBuffer()
    {
        return IdStagingBuffer;
    }

private:

    template <typename T>
    void CBufferUpdate(ID3D11Buffer* CBuffer, T& CBufferData)
    {
        D3D11_MAPPED_SUBRESOURCE MSR;

        //gpu메모리를 cpu가 접근할 수 있도록 잠금
        //D3D11_MAP_WRITE_DISCARD : gpu안의 데이터를 버리고 cpu에서 넣는 데이터로 덮어씌움 
        //gpu내용을 버림으로써 gpu안의 메모리를 읽는 과정 생략
        DeviceContext->Map(CBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &MSR);
        memcpy(MSR.pData, &CBufferData, sizeof(T));
        DeviceContext->Unmap(CBuffer, 0);
    }
    void CBufferSet(ID3D11Buffer* CBuffer, const uint32 SlotNum, const bool bSetVS, const  bool bSetPS)
    {
        if (bSetVS)
        {
            DeviceContext->VSSetConstantBuffers(SlotNum, 1, &CBuffer);
        }
        if (bSetPS)
        {
            DeviceContext->PSSetConstantBuffers(SlotNum, 1, &CBuffer);
        }
    }

    template <typename T>
    void CBufferUpdateSet(ID3D11Buffer* CBuffer, T& CBufferData, const uint32 SlotNum, const bool bSetVS, const bool bSetPS)
    {
        CBufferUpdate(CBuffer, CBufferData);
        CBufferSet(CBuffer, SlotNum, bSetVS, bSetPS);
    }

    void CreateCBuffer(ID3D11Buffer** CBuffer, const uint32 Size)
    {
        D3D11_BUFFER_DESC CBufferDesc = {};
        CBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        CBufferDesc.ByteWidth = (Size + 15) & ~15;
        CBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        CBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        Device->CreateBuffer(&CBufferDesc, nullptr, CBuffer);
    }

    void CreateDeviceAndSwapChain(HWND hWindow)override; // 여기서 디바이스, 디바이스 컨택스트, 스왑체인, 뷰포트를 초기화한다
    void CreateFrameBuffer() override;
    void CreateRasterizerState() override;
    void CreateConstantBuffer() override;
    void CreateDepthStencilState() override;
	void CreateSamplerState();
    void CreateIdBuffer();

    // release
	void ReleaseSamplerState();
    void ReleaseBlendState();
    void ReleaseRasterizerState(); // rs
    void ReleaseFrameBuffer(); // fb, rtv
    void ReleaseDeviceAndSwapChain();
    void ReleaseIdBuffer();
 
	void OmSetDepthStencilState(EComparisonFunc Func) override;
    
private:
    //24
    D3D11_VIEWPORT ViewportInfo{};

    //8
    ID3D11Device* Device{};//
    ID3D11DeviceContext* DeviceContext{};//
    IDXGISwapChain* SwapChain{};//

    ID3D11RasterizerState* DefaultRasterizerState{};//
    ID3D11RasterizerState* WireFrameRasterizerState{};//
    ID3D11RasterizerState* FrontCullRasterizerState{};//
    ID3D11RasterizerState* NoCullRasterizerState{};//
    ID3D11RasterizerState* DecalRasterizerState{};// DepthBias 적용된 데칼용 state

    ID3D11DepthStencilState* DepthStencilState{};

    ID3D11DepthStencilState* DepthStencilStateLessEqualWrite = nullptr;      // 기본
    ID3D11DepthStencilState* DepthStencilStateLessEqualReadOnly = nullptr;   // 읽기 전용
    ID3D11DepthStencilState* DepthStencilStateAlwaysNoWrite = nullptr;       // 기즈모/오버레이
    ID3D11DepthStencilState* DepthStencilStateDisable = nullptr;              // 깊이 테스트/쓰기 모두 끔
    ID3D11DepthStencilState* DepthStencilStateGreaterEqualWrite = nullptr;   // 선택사항

    ID3D11BlendState* BlendStateOpaque{};
    ID3D11BlendState* BlendStateTransparent{};

    ID3D11Texture2D* FrameBuffer{};//
    ID3D11Texture2D* IdBuffer = nullptr;
    ID3D11Texture2D* IdStagingBuffer = nullptr;

    ID3D11RenderTargetView* IdBufferRTV = nullptr;
    ID3D11RenderTargetView* RenderTargetView{};//

    ID3D11DepthStencilView* DepthStencilView{};//
    ID3D11ShaderResourceView* DepthSRV{}; // Depth buffer를 셰이더에서 읽기 위한 SRV


    CBUFFER_TYPE_LIST(DECLARE_CBUFFER)

    // 버퍼 핸들
    ID3D11Buffer* ModelCB{};
    ID3D11Buffer* ViewProjCB{};
    ID3D11Buffer* HighLightCB{};
    ID3D11Buffer* BillboardCB{};
    ID3D11Buffer* ColorCB{};
    ID3D11Buffer* PixelConstCB{};
    ID3D11Buffer* UVScrollCB{};
    ID3D11Buffer* InvWorldCB{};
    ID3D11Buffer* ViewportCB{};
    ID3D11Buffer* DecalCB{};

    ID3D11Buffer* ConstantBuffer{};

    ID3D11SamplerState* DefaultSamplerState = nullptr;

    
};


template<typename TVertex>
inline HRESULT D3D11RHI::CreateVertexBufferImpl(ID3D11Device* device, const FMeshData& mesh, ID3D11Buffer** outBuffer, D3D11_USAGE usage, UINT cpuAccessFlags)
{
    std::vector<TVertex> vertexArray;
    vertexArray.reserve(mesh.Vertices.size());

    for (size_t i = 0; i < mesh.Vertices.size(); ++i) {
        TVertex vtx{};
        vtx.FillFrom(mesh, i);
        vertexArray.push_back(vtx);
    }

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = usage;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = cpuAccessFlags;
    vbd.ByteWidth = static_cast<UINT>(sizeof(TVertex) * vertexArray.size());

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = vertexArray.data();

    return device->CreateBuffer(&vbd, &vinitData, outBuffer);
}

// PositionColor → Static
template<>
inline HRESULT D3D11RHI::CreateVertexBuffer<FVertexSimple>(ID3D11Device* device, const FMeshData& mesh, ID3D11Buffer** outBuffer)
{
    return CreateVertexBufferImpl<FVertexSimple>(device, mesh, outBuffer,
        D3D11_USAGE_DEFAULT, 0);
}

// PositionColorTextureNormal → Static
template<>
inline HRESULT D3D11RHI::CreateVertexBuffer<FVertexDynamic>(ID3D11Device* device, const FMeshData& mesh, ID3D11Buffer** outBuffer)
{
    return CreateVertexBufferImpl<FVertexDynamic>(device, mesh, outBuffer,
        D3D11_USAGE_DEFAULT, 0);
}

// Billboard → Dynamic
template<>
inline HRESULT D3D11RHI::CreateVertexBuffer<FBillboardVertexInfo_GPU>(ID3D11Device* device, const FMeshData& mesh, ID3D11Buffer** outBuffer)
{
    return CreateVertexBufferImpl<FBillboardVertexInfo_GPU>(device, mesh, outBuffer,
        D3D11_USAGE_DYNAMIC,
        D3D11_CPU_ACCESS_WRITE);
}

template<typename TVertex>
inline HRESULT D3D11RHI::CreateVertexBufferImpl(ID3D11Device* device, const std::vector<FNormalVertex>& srcVertices, ID3D11Buffer** outBuffer, D3D11_USAGE usage, UINT cpuAccessFlags)
{
    std::vector<TVertex> vertexArray;
    vertexArray.reserve(srcVertices.size());

    for (size_t i = 0; i < srcVertices.size(); ++i)
    {
        TVertex vtx{};
        vtx.FillFrom(srcVertices[i]); // 각 TVertex에서 FillFrom 구현 필요
        vertexArray.push_back(vtx);
    }

    D3D11_BUFFER_DESC vbd = {};
    vbd.Usage = usage;
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = cpuAccessFlags;
    vbd.ByteWidth = static_cast<UINT>(sizeof(TVertex) * vertexArray.size());

    D3D11_SUBRESOURCE_DATA vinitData = {};
    vinitData.pSysMem = vertexArray.data();

    return device->CreateBuffer(&vbd, &vinitData, outBuffer);
}

template<>
inline HRESULT D3D11RHI::CreateVertexBuffer<FVertexSimple>(ID3D11Device* device, const std::vector<FNormalVertex>& srcVertices, ID3D11Buffer** outBuffer)
{
    return CreateVertexBufferImpl<FVertexSimple>(device, srcVertices, outBuffer, D3D11_USAGE_DEFAULT, 0);
}

// PositionColorTextureNormal
template<>
inline HRESULT D3D11RHI::CreateVertexBuffer<FVertexDynamic>(ID3D11Device* device, const std::vector<FNormalVertex>& srcVertices, ID3D11Buffer** outBuffer)
{
    return CreateVertexBufferImpl<FVertexDynamic>(device, srcVertices, outBuffer, D3D11_USAGE_DEFAULT, 0);
}

// Billboard
template<>
inline HRESULT D3D11RHI::CreateVertexBuffer<FBillboardVertexInfo_GPU>(ID3D11Device* device, const std::vector<FNormalVertex>& srcVertices, ID3D11Buffer** outBuffer)
{
    return CreateVertexBufferImpl<FBillboardVertexInfo_GPU>(device, srcVertices, outBuffer, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE);
}
