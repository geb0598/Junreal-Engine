﻿#pragma once
#include "BillboardComponent.h"
#include "RHIDevice.h"
#include "LineDynamicMesh.h"
#include "FXAA.h"

class UStaticMeshComponent;
class UTextRenderComponent;
class UMeshComponent;
class URHIDevice;
class UShader;
class UStaticMesh;
struct FMaterialSlot;


//여긴 템플릿 써도 되는데 통일성을 위해 매크로로 적용
#define DECLARE_CBUFFER_UPDATE_FUNC(TYPE)\
   void UpdateCBuffer(const TYPE& CBufferData)\
{\
    RHIDevice->UpdateCBuffer(CBufferData);\
}
#define DECLARE_CBUFFER_UPDATE_SET_FUNC(TYPE)\
void UpdateSetCBuffer(const TYPE& CBufferData)\
{\
    RHIDevice->UpdateSetCBuffer(CBufferData);\
}
#define DECLARE_CBUFFER_SET_FUNC(TYPE)\
void SetCBuffer(const TYPE& CBufferData)\
{\
    RHIDevice->SetCBuffer(CBufferData);\
}



class URenderer
{
public:
    URenderer(URHIDevice* InDevice);

    ~URenderer();

public:
    void Update(float DeltaSecond);

	void BeginFrame();

    void PrepareShader(UShader* InShader);

    void OMSetBlendState(bool bIsChecked);

    void RSSetState(EViewModeIndex ViewModeIndex);

    void RSSetFrontCullState();

    void RSSetNoCullState();

    void RSSetDefaultState();

    void RenderFrame(class UWorld* World);   //패스를 위한 렌더프레임

    CBUFFER_TYPE_LIST(DECLARE_CBUFFER_UPDATE_FUNC)
        CBUFFER_TYPE_LIST(DECLARE_CBUFFER_UPDATE_SET_FUNC)
        CBUFFER_TYPE_LIST(DECLARE_CBUFFER_SET_FUNC)

    void DrawIndexedPrimitiveComponent(UStaticMesh* InMesh, D3D11_PRIMITIVE_TOPOLOGY InTopology, const TArray<FMaterialSlot>& InComponentMaterialSlots);


    void DrawIndexedPrimitiveComponent(UTextRenderComponent* Comp, D3D11_PRIMITIVE_TOPOLOGY InTopology);
    void DrawIndexedPrimitiveComponent(UBillboardComponent* Comp,
                                       D3D11_PRIMITIVE_TOPOLOGY InTopology);

    void SetViewModeType(EViewModeIndex ViewModeIndex);
    void SetViewModeIndex(EViewModeIndex InViewModeIndex) { ViewModeIndex = InViewModeIndex; }
    // Batch Line Rendering System
    void BeginLineBatch();
    void AddLine(const FVector& Start, const FVector& End, const FVector4& Color = FVector4(1.0f, 1.0f, 1.0f, 1.0f));
    void AddLines(const TArray<FVector>& StartPoints, const TArray<FVector>& EndPoints, const TArray<FVector4>& Colors);
    void AddLines(const TArray<FVector>& LineList, const FVector4& Color);
    void EndLineBatch(const FMatrix& ModelMatrix, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix);
    UPrimitiveComponent* GetCollidedPrimitive(int MouseX, int MouseY) const;
    void ClearLineBatch();

	void EndFrame();

    void OMSetDepthStencilState(EComparisonFunc Func);

    void RenderViewPorts(UWorld* World);

    URHIDevice* GetRHIDevice() { return RHIDevice; }
    void RenderScene(UWorld* World, ACameraActor* Camera, FViewport* Viewport);
    void RenderPostProcessing(UShader* Shader);

private:

    // ========== 핵심: 패스 분리 ==========

    // 1) 패스들
    void RenderBasePass(UWorld* World, ACameraActor* Camera, FViewport* Viewport);         // 불투명/기본 머티리얼
    void RenderFogPass(UWorld* World, ACameraActor* Camera, FViewport* Viewport);                       // 포스트: SceneColor/SceneDepth 기반
    void RenderFireBallPass(UWorld* World);     // 포스트: FireBall 조명/가산
    void RenderOverlayPass(UWorld* World);      // 라인/텍스트/UI/디버그

    // 2) 씬 렌더링 헬퍼 메소드들

    void RenderEditorPass(UWorld* World, ACameraActor* Camera, FViewport* Viewport);

    void RenderActorsInViewport(UWorld* World, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, FViewport* Viewport);
    void RenderPrimitives(UWorld* World, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, FViewport* Viewport);
    void RenderDecals(UWorld* World, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, FViewport* Viewport);
    void RenderEngineActors(const TArray<AActor*>& EngineActors, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, FViewport* Viewport);

    //// 2) 풀스크린 쿼드
    //void CreateFullScreenQuad();
    //void DestroyFullScreenQuad();
    //void DrawFullScreenQuad();

    //// 3) 씬 타겟 (SceneColor/SceneDepth)
    //void EnsureSceneTargets();     // 뷰포트 크기 반영해 생성/재생성
    //void ReleaseSceneTargets();

    // 4) 파이프라인 공통 상수
    struct alignas(16) FPostCB { FVector4 ViewSize; FVector2D NearFar; FVector2D Pad; };
    struct alignas(16) FViewProjCB { FMatrix View; FMatrix Proj; FMatrix InvViewProj; };

    // ========== 내부 상태 ==========

    EViewModeIndex ViewModeIndex = EViewModeIndex::VMI_Unlit;

	URHIDevice* RHIDevice;

    TArray<uint32> IdBufferCache;
    // Batch Line Rendering System using UDynamicMesh for efficiency
    ULineDynamicMesh* DynamicLineMesh = nullptr;
    FMeshData* LineBatchData = nullptr;
    UShader* LineShader = nullptr;
    bool bLineBatchActive = false;
    static const uint32 MAX_LINES = 10000;  // Maximum lines per batch
    
    // 렌더링 통계를 위한 상태 추적
    UMaterial* LastMaterial = nullptr;
    UShader* LastShader = nullptr;
    UTexture* LastTexture = nullptr;

    /**
     * @brief 불필요한 API 호출을 막기 위해 마지막으로 바인딩된 상태를 캐싱합니다.
     */
    ID3D11Buffer* LastVertexBuffer = nullptr;
    ID3D11Buffer* LastIndexBuffer = nullptr;
    D3D11_PRIMITIVE_TOPOLOGY LastPrimitiveTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11ShaderResourceView* LastTextureSRV = nullptr;

    UFXAA FXAA;

    void InitializeLineBatch();
    void ResetRenderStateTracking();
};

