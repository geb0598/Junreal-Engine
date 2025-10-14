#include "pch.h"

#include "BillboardComponent.h"
#include "TextRenderComponent.h"
#include "Shader.h"
#include "StaticMesh.h"
#include "TextQuad.h"
#include "StaticMeshComponent.h"
#include "RenderingStats.h"
#include "UI/StatsOverlayD2D.h"
#include "SMultiViewportWindow.h"
#include "SelectionManager.h"
#include "DecalComponent.h"
#include "SpotLightComponent.h"
#include "GridActor.h"
#include "FViewport.h"
#include "CameraActor.h"
#include "Frustum.h"
#include "BoundingVolume.h"
#include "GizmoActor.h"
#include "FireballComponent.h"


URenderer::URenderer(URHIDevice* InDevice) : RHIDevice(InDevice)
{
    InitializeLineBatch();
}

URenderer::~URenderer()
{
    if (LineBatchData)
    {
        delete LineBatchData;
    }
}

void URenderer::Update(float DeltaSeconds)
{
    
}
void URenderer::BeginFrame()
{
    // 렌더링 통계 수집 시작
    URenderingStatsCollector::GetInstance().BeginFrame();
    
    // 상태 추적 리셋
    ResetRenderStateTracking();
    
    // 백버퍼/깊이버퍼를 클리어
    RHIDevice->ClearBackBuffer();  // 배경색
    RHIDevice->ClearDepthBuffer(1.0f, 0);                 // 깊이값 초기화
    //RHIDevice->CreateBlendState();
    RHIDevice->IASetPrimitiveTopology();
    // RS
    RHIDevice->RSSetViewport();

    //OM
    //RHIDevice->OMSetBlendState();
    RHIDevice->OMSetRenderTargets();
}

void URenderer::PrepareShader(UShader* InShader)
{
    // 셰이더 변경 추적
    if (LastShader != InShader)
    {
        URenderingStatsCollector::GetInstance().IncrementShaderChanges();
        LastShader = InShader;
    }
    
    RHIDevice->GetDeviceContext()->VSSetShader(InShader->GetVertexShader(), nullptr, 0);
    RHIDevice->GetDeviceContext()->PSSetShader(InShader->GetPixelShader(), nullptr, 0);
    RHIDevice->GetDeviceContext()->IASetInputLayout(InShader->GetInputLayout());
}

void URenderer::OMSetBlendState(bool bIsChecked)
{
    if (bIsChecked == true)
    {
        RHIDevice->OMSetBlendState(true);
    }
    else
    {
        RHIDevice->OMSetBlendState(false);
    }
}

void URenderer::RSSetState(EViewModeIndex ViewModeIndex)
{
    RHIDevice->RSSetState(ViewModeIndex);
}

void URenderer::RSSetFrontCullState()
{
    RHIDevice->RSSetFrontCullState();
}

void URenderer::RSSetNoCullState()
{
    RHIDevice->RSSetNoCullState();
}

void URenderer::RSSetDefaultState()
{
    RHIDevice->RSSetDefaultState();
}

void URenderer::RenderFrame(UWorld* World)
{
    BeginFrame();
    UUIManager::GetInstance().Render();

    RenderViewPorts(World);

    UUIManager::GetInstance().EndFrame();
    EndFrame();
}

void URenderer::DrawIndexedPrimitiveComponent(UStaticMesh* InMesh, D3D11_PRIMITIVE_TOPOLOGY InTopology, const TArray<FMaterialSlot>& InComponentMaterialSlots)
{
    URenderingStatsCollector& StatsCollector = URenderingStatsCollector::GetInstance();
    
    // 디버그: StaticMesh 렌더링 통계
    
    UINT stride = 0;
    switch (InMesh->GetVertexType())
    {
    case EVertexLayoutType::PositionColor:
        stride = sizeof(FVertexSimple);
        break;
    case EVertexLayoutType::PositionColorTexturNormal:
        stride = sizeof(FVertexDynamic);
        break;
    case EVertexLayoutType::PositionBillBoard:
        stride = sizeof(FBillboardVertexInfo_GPU);
        break;
    case EVertexLayoutType::PositionUV:
        stride = sizeof(FVertexUV);
    default:
        // Handle unknown or unsupported vertex types
        assert(false && "Unknown vertex type!");
        return; // or log an error
    }
    UINT offset = 0;

    ID3D11Buffer* VertexBuffer = InMesh->GetVertexBuffer();
    ID3D11Buffer* IndexBuffer = InMesh->GetIndexBuffer();
    uint32 VertexCount = InMesh->GetVertexCount();
    uint32 IndexCount = InMesh->GetIndexCount();

    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &VertexBuffer, &stride, &offset
    );

    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        IndexBuffer, DXGI_FORMAT_R32_UINT, 0
    );

    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(InTopology);
    RHIDevice->PSSetDefaultSampler(0);

    if (InMesh->HasMaterial())
    {
        const TArray<FGroupInfo>& MeshGroupInfos = InMesh->GetMeshGroupInfo();
        const uint32 NumMeshGroupInfos = static_cast<uint32>(MeshGroupInfos.size());
        for (uint32 i = 0; i < NumMeshGroupInfos; ++i)
        {
            const UMaterial* const Material = UResourceManager::GetInstance().Get<UMaterial>(InComponentMaterialSlots[i].MaterialName);
            const FObjMaterialInfo& MaterialInfo = Material->GetMaterialInfo();
            bool bHasTexture = !(MaterialInfo.DiffuseTextureFileName == FName::None());
            
            // 재료 변경 추적
            if (LastMaterial != Material)
            {
                StatsCollector.IncrementMaterialChanges();
                LastMaterial = const_cast<UMaterial*>(Material);
            }
            
            FTextureData* TextureData = nullptr;
            if (bHasTexture)
            {
                TextureData = UResourceManager::GetInstance().CreateOrGetTextureData(MaterialInfo.DiffuseTextureFileName);
                
                // 텍스처 변경 추적 (임시로 FTextureData*를 UTexture*로 캠스트)
                UTexture* CurrentTexture = reinterpret_cast<UTexture*>(TextureData);
                if (LastTexture != CurrentTexture)
                {
                    StatsCollector.IncrementTextureChanges();
                    LastTexture = CurrentTexture;
                }
                
                RHIDevice->GetDeviceContext()->PSSetShaderResources(0, 1, &(TextureData->TextureSRV));
            }
            
            RHIDevice->UpdateSetCBuffer(FPixelConstBufferType(FMaterialInPs(MaterialInfo), true, bHasTexture)); // PSSet도 해줌
            
            // DrawCall 수실행 및 통계 추가
            RHIDevice->GetDeviceContext()->DrawIndexed(MeshGroupInfos[i].IndexCount, MeshGroupInfos[i].StartIndex, 0);
            StatsCollector.IncrementDrawCalls();
        }
    }
    else
    {
        FObjMaterialInfo ObjMaterialInfo;
        RHIDevice->UpdateSetCBuffer(FPixelConstBufferType(FMaterialInPs(ObjMaterialInfo), false, false)); // PSSet도 해줌
        RHIDevice->GetDeviceContext()->DrawIndexed(IndexCount, 0, 0);
        StatsCollector.IncrementDrawCalls();
    }
}

void URenderer::DrawIndexedPrimitiveComponent(UTextRenderComponent* Comp, D3D11_PRIMITIVE_TOPOLOGY InTopology)
{
    URenderingStatsCollector& StatsCollector = URenderingStatsCollector::GetInstance();
    
    // 디버그: TextRenderComponent 렌더링 통계
    
    UINT Stride = sizeof(FBillboardVertexInfo_GPU);
    ID3D11Buffer* VertexBuff = Comp->GetStaticMesh()->GetVertexBuffer();
    ID3D11Buffer* IndexBuff = Comp->GetStaticMesh()->GetIndexBuffer();

    // 매테리얼 변경 추적
    UMaterial* CompMaterial = Comp->GetMaterial();
    if (LastMaterial != CompMaterial)
    {
        StatsCollector.IncrementMaterialChanges();
        LastMaterial = CompMaterial;
    }
    
    UShader* CompShader = CompMaterial->GetShader();
    // 셰이더 변경 추적
    if (LastShader != CompShader)
    {
        StatsCollector.IncrementShaderChanges();
        LastShader = CompShader;
    }
    
    RHIDevice->GetDeviceContext()->IASetInputLayout(CompShader->GetInputLayout());

    
    UINT offset = 0;
    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &VertexBuff, &Stride, &offset
    );
    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        IndexBuff, DXGI_FORMAT_R32_UINT, 0
    );

    // 텍스처 변경 추적 (텍스처 비교)
    UTexture* CompTexture = CompMaterial->GetTexture();
    if (LastTexture != CompTexture)
    {
        StatsCollector.IncrementTextureChanges();
        LastTexture = CompTexture;
    }
    
    ID3D11ShaderResourceView* TextureSRV = CompTexture->GetShaderResourceView();
    RHIDevice->PSSetDefaultSampler(0);
    RHIDevice->GetDeviceContext()->PSSetShaderResources(0, 1, &TextureSRV);
    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(InTopology);
    RHIDevice->GetDeviceContext()->DrawIndexed(Comp->GetStaticMesh()->GetIndexCount(), 0, 0);
    StatsCollector.IncrementDrawCalls();
}


void URenderer::DrawIndexedPrimitiveComponent(UBillboardComponent* Comp, D3D11_PRIMITIVE_TOPOLOGY InTopology)
{
    URenderingStatsCollector& StatsCollector = URenderingStatsCollector::GetInstance();
    
    // 디버그: TextRenderComponent 렌더링 통계
    
    UINT Stride = sizeof(FBillboardVertexInfo_GPU);
    ID3D11Buffer* VertexBuff = Comp->GetStaticMesh()->GetVertexBuffer();
    ID3D11Buffer* IndexBuff = Comp->GetStaticMesh()->GetIndexBuffer();

    // 매테리얼 변경 추적
    UMaterial* CompMaterial = Comp->GetMaterial();
    if (LastMaterial != CompMaterial)
    {
        StatsCollector.IncrementMaterialChanges();
        LastMaterial = CompMaterial;
    }
    
    UShader* CompShader = CompMaterial->GetShader();
    // 셰이더 변경 추적
    if (LastShader != CompShader)
    {
        StatsCollector.IncrementShaderChanges();
        LastShader = CompShader;
    }
    
    RHIDevice->GetDeviceContext()->IASetInputLayout(CompShader->GetInputLayout());

    
    UINT offset = 0;
    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &VertexBuff, &Stride, &offset
    );
    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        IndexBuff, DXGI_FORMAT_R32_UINT, 0
    );

    // 텍스처 변경 추적 (텍스처 비교)
    UTexture* CompTexture = CompMaterial->GetTexture();
    if (LastTexture != CompTexture)
    {
        StatsCollector.IncrementTextureChanges();
        LastTexture = CompTexture;
    }
    
    ID3D11ShaderResourceView* TextureSRV = CompTexture->GetShaderResourceView();
    RHIDevice->PSSetDefaultSampler(0);
    RHIDevice->GetDeviceContext()->PSSetShaderResources(0, 1, &TextureSRV);
    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(InTopology);
    RHIDevice->GetDeviceContext()->DrawIndexed(Comp->GetStaticMesh()->GetIndexCount(), 0, 0);
    StatsCollector.IncrementDrawCalls();
}

void URenderer::SetViewModeType(EViewModeIndex ViewModeIndex)
{
    RHIDevice->RSSetState(ViewModeIndex);
    if(ViewModeIndex == EViewModeIndex::VMI_Wireframe)
        RHIDevice->UpdateSetCBuffer(ColorBufferType{ FVector4(1.f, 0.f, 0.f, 1.f) });
    else
        RHIDevice->UpdateSetCBuffer(ColorBufferType{ FVector4(1.f, 1.f, 1.f, 0.f) });
}

void URenderer::EndFrame()
{

    
    // 렌더링 통계 수집 종료
    URenderingStatsCollector& StatsCollector = URenderingStatsCollector::GetInstance();
    StatsCollector.EndFrame();
    
    // 현재 프레임 통계를 업데이트
    const FRenderingStats& CurrentStats = StatsCollector.GetCurrentFrameStats();
    StatsCollector.UpdateFrameStats(CurrentStats);
    
    // 평균 통계를 얻어서 오버레이에 업데이트
    const FRenderingStats& AvgStats = StatsCollector.GetAverageStats();
    UStatsOverlayD2D::Get().UpdateRenderingStats(
        AvgStats.TotalDrawCalls,
        AvgStats.MaterialChanges,
        AvgStats.TextureChanges,
        AvgStats.ShaderChanges
    );
    
    RHIDevice->Present();
}

void URenderer::OMSetDepthStencilState(EComparisonFunc Func)
{
    RHIDevice->OmSetDepthStencilState(Func);
}

void URenderer::RenderViewPorts(UWorld* World) 
{
    // 멀티 뷰포트 시스템을 통해 각 뷰포트별로 렌더링
    if (SMultiViewportWindow* MultiViewport = World->GetMultiViewportWindow())
    {
        MultiViewport->OnRender();
    }
}


void URenderer::RenderBasePass(UWorld* World, ACameraActor* Camera, FViewport* Viewport)
{
  

    // 뷰포트의 실제 크기로 aspect ratio 계산
    float ViewportAspectRatio = static_cast<float>(Viewport->GetSizeX()) / static_cast<float>(Viewport->GetSizeY());
    if (Viewport->GetSizeY() == 0)
    {
        ViewportAspectRatio = 1.0f;
    }

    FMatrix ViewMatrix = Camera->GetViewMatrix();
    FMatrix ProjectionMatrix = Camera->GetProjectionMatrix(ViewportAspectRatio, Viewport);

    // 씬의 액터들을 렌더링
    RenderActorsInViewport(World, ViewMatrix, ProjectionMatrix, Viewport);

    // Gizmo 렌더링 (에디터 전용)
    if (!World->IsPIEWorld())
    {
        if (AGizmoActor* Gizmo = World->GetGizmoActor())
        {
            Gizmo->Render(Camera, Viewport);
        }
    }

}


void URenderer::RenderScene(UWorld* World, ACameraActor* Camera, FViewport* Viewport)
{

    // 렌더 패스 구조:
    RenderFireBallPass(World);
    // 2. Base Pass (Opaque geometry - 각 뷰포트별로)
    // 2. Post-processing passes
    RenderBasePass(World, Camera, Viewport);

    RenderFogPass();

    // 4. Overlay (UI, debug visualization)
    RenderOverlayPass(World);

    
    if (!World || !Camera || !Viewport)
    {
        return;
    }

  
}

void URenderer::RenderActorsInViewport(UWorld* World, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, FViewport* Viewport)
{
    if (!World || !Viewport)
    {
        return;
    }

    FFrustum ViewFrustum;
    ViewFrustum.Update(ViewMatrix * ProjectionMatrix);

    BeginLineBatch();
    SetViewModeType(ViewModeIndex);

    const TArray<AActor*>& LevelActors = World->GetLevel() ? World->GetLevel()->GetActors() : TArray<AActor*>();
    USelectionManager& SelectionManager = USelectionManager::GetInstance();

    // 특수 처리가 필요한 컴포넌트들
    TArray<UDecalComponent*> Decals;
    TArray<UPrimitiveComponent*> RenderPrimitivesWithOutDecal;
    TArray<UBillboardComponent*> BillboardComponentList;

    if (!Viewport->IsShowFlagEnabled(EEngineShowFlags::SF_Primitives))
    {
        return;
    }

    // 액터별로 순회하며 렌더링
    for (AActor* Actor : LevelActors)
    {
        if (!Actor || Actor->GetActorHiddenInGame())
        {
            continue;
        }

        bool bIsSelected = SelectionManager.IsActorSelected(Actor);

        for (UActorComponent* ActorComp : Actor->GetComponents())
        {
            if (!ActorComp)
            {
                continue;
            }

            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(ActorComp))
            {
                // 바운딩 박스 그리기
                if (Viewport->IsShowFlagEnabled(EEngineShowFlags::SF_BoundingBoxes))
                {
                    AddLines(Primitive->GetBoundingBoxLines(), Primitive->GetBoundingBoxColor());
                }

                // 데칼 컴포넌트는 나중에 처리
                if (UDecalComponent* Decal = Cast<UDecalComponent>(ActorComp))
                {
                    Decals.Add(Decal);
                    continue;
                }
                if (UBillboardComponent* Billboard = Cast<UBillboardComponent>(ActorComp))
                {
                    BillboardComponentList.Add(Billboard);
                    continue;
                }

                RenderPrimitivesWithOutDecal.Add(Primitive);

                FVector rgb(1.0f, 1.0f, 1.0f);
                UpdateSetCBuffer(HighLightBufferType(bIsSelected, rgb, 0, 0, 0, 0));
                Primitive->Render(this, ViewMatrix, ProjectionMatrix, Viewport->GetShowFlags());
            }
        }
    }

    OMSetBlendState(false);
    RenderEngineActors(World->GetEngineActors(), ViewMatrix, ProjectionMatrix, Viewport);

    // 데칼 렌더링
    if (Viewport->IsShowFlagEnabled(EEngineShowFlags::SF_Decals))
    {
        Decals.Sort([](const UDecalComponent* A, const UDecalComponent* B)
        {
            return A->GetSortOrder() < B->GetSortOrder();
        });

        for (UDecalComponent* Decal : Decals)
        {
            FOBB DecalWorldOBB = Decal->GetWorldOBB();

            if (World->GetUseBVH() && World->GetBVH().IsBuild())
            {
                TArray<UPrimitiveComponent*> CollisionPrimitives = World->GetBVH().GetCollisionWithOBB(DecalWorldOBB);
                for (UPrimitiveComponent* Primitive : CollisionPrimitives)
                {
                    Decal->Render(this, Primitive, ViewMatrix, ProjectionMatrix, Viewport);
                }
            }
            else
            {
                for (UPrimitiveComponent* Primitive : RenderPrimitivesWithOutDecal)
                {
                    if (IntersectOBBAABB(DecalWorldOBB, Primitive->GetWorldAABB()))
                    {
                        Decal->Render(this, Primitive, ViewMatrix, ProjectionMatrix, Viewport);
                    }
                }
            }
        }
    }

    // BVH 바운드 시각화
    if (Viewport->IsShowFlagEnabled(EEngineShowFlags::SF_BVH))
    {
        AddLines(World->GetBVH().GetBVHBoundsWire(), FVector4(0.5f, 0.5f, 1, 1));
    }

    EndLineBatch(FMatrix::Identity(), ViewMatrix, ProjectionMatrix);

    // 빌보드는 마지막에 렌더링
    for (auto& Billboard : BillboardComponentList)
    {
        Billboard->Render(this, ViewMatrix, ProjectionMatrix, Viewport->GetShowFlags());
    }

  
}

void URenderer::RenderEngineActors(const TArray<AActor*>& EngineActors, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix, FViewport* Viewport)
{
    for (AActor* EngineActor : EngineActors)
    {
        if (!EngineActor || EngineActor->GetActorHiddenInGame())
        {
            continue;
        }

        if (Cast<AGridActor>(EngineActor) && !Viewport->IsShowFlagEnabled(EEngineShowFlags::SF_Grid))
        {
            continue;
        }

        for (UActorComponent* Component : EngineActor->GetComponents())
        {
            if (!Component)
            {
                continue;
            }

            if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
            {
                if (!ActorComp->IsActive())
                {
                    continue;
                }
            }

            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                SetViewModeType(EViewModeIndex::VMI_Unlit);
                Primitive->Render(this, ViewMatrix, ProjectionMatrix, Viewport->GetShowFlags());
                OMSetDepthStencilState(EComparisonFunc::LessEqual);
            }
        }
        OMSetBlendState(false);
    }
}

void URenderer::RenderPostProcessing(UShader* Shader)
{
    OMSetBlendState(false);
    OMSetDepthStencilState(EComparisonFunc::Disable);
    PrepareShader(Shader);
    UINT Stride = sizeof(FVertexUV);
    UINT Offset = 0;
    UStaticMesh* StaticMesh = UResourceManager::GetInstance().Load<UStaticMesh>("ScreenQuad");
    ID3D11Buffer* VertexBuffer = StaticMesh->GetVertexBuffer();
    ID3D11Buffer* IndexBuffer = StaticMesh->GetIndexBuffer();
    RHIDevice->GetDeviceContext()->IASetVertexBuffers(
        0, 1, &VertexBuffer, &Stride, &Offset
    );

    RHIDevice->GetDeviceContext()->IASetIndexBuffer(
        IndexBuffer, DXGI_FORMAT_R32_UINT, 0
    );
    RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    RHIDevice->PSSetDefaultSampler(0);

    RHIDevice->GetDeviceContext()->DrawIndexed(StaticMesh->GetIndexCount(), 0, 0);

    
}

void URenderer::RenderFogPass()
{
    // TODO: 화면 전체 Fog 효과 구현
}

void URenderer::RenderFireBallPass(UWorld* World)
{
    if (!World) return;

    // 1️⃣ 라이트 컴포넌트 수집 (FireBall, PointLight 등)
    FPointLightBufferType PointLightCB{};
    const TArray<AActor*>& Actors = World->GetLevel()->GetActors();

    for (AActor* Actor : Actors)
    {
        if (!Actor) continue;
        for (UActorComponent* Comp : Actor->GetComponents())
        {
            if (UPrimitiveComponent* Prim = Cast<UPrimitiveComponent>(Comp))
            {
                if (UFireBallComponent* Fire = Cast<UFireBallComponent>(Prim))
                {
                    int idx = PointLightCB.PointLightCount++;
                    if (idx >= MAX_POINT_LIGHTS) break;

                    PointLightCB.PointLights[idx].Position = FVector4(
                        Fire->GetWorldLocation(), Fire->Radius
                    );
                    PointLightCB.PointLights[idx].Color = FVector4(
                        Fire->Color.R, Fire->Color.G, Fire->Color.B, Fire->Intensity
                    );
                    PointLightCB.PointLights[idx].FallOff = Fire->RadiusFallOff;
                }
            }
        }
    }

    // 2️⃣ 상수 버퍼 GPU로 업데이트
    UpdateSetCBuffer(PointLightCB);
}

void URenderer::RenderOverlayPass(UWorld* World)
{
    // TODO: 오버레이(UI, 디버그 텍스트 등) 구현
}

void URenderer::InitializeLineBatch()
{
    // Create UDynamicMesh for efficient line batching
    DynamicLineMesh = UResourceManager::GetInstance().Load<ULineDynamicMesh>("Line");
    
    // Initialize with maximum capacity (MAX_LINES * 2 vertices, MAX_LINES * 2 indices)
    uint32 maxVertices = MAX_LINES * 2;
    uint32 maxIndices = MAX_LINES * 2;
    DynamicLineMesh->Load(maxVertices, maxIndices, RHIDevice->GetDevice());

    // Create FMeshData for accumulating line data
    LineBatchData = new FMeshData();
    
    // Load line shader
    LineShader = UResourceManager::GetInstance().Load<UShader>("ShaderLine.hlsl");
}

void URenderer::BeginLineBatch()
{
    if (!LineBatchData) return;
    
    bLineBatchActive = true;
    
    // Clear previous batch data
    LineBatchData->Vertices.clear();
    LineBatchData->Color.clear();
    LineBatchData->Indices.clear();
}

void URenderer::AddLine(const FVector& Start, const FVector& End, const FVector4& Color)
{
    if (!bLineBatchActive || !LineBatchData) return;
    
    uint32 startIndex = static_cast<uint32>(LineBatchData->Vertices.size());
    
    // Add vertices
    LineBatchData->Vertices.push_back(Start);
    LineBatchData->Vertices.push_back(End);
    
    // Add colors
    LineBatchData->Color.push_back(Color);
    LineBatchData->Color.push_back(Color);
    
    // Add indices for line (2 vertices per line)
    LineBatchData->Indices.push_back(startIndex);
    LineBatchData->Indices.push_back(startIndex + 1);
}

void URenderer::AddLines(const TArray<FVector>& StartPoints, const TArray<FVector>& EndPoints, const TArray<FVector4>& Colors)
{
    if (!bLineBatchActive || !LineBatchData) return;
    
    // Validate input arrays have same size
    if (StartPoints.size() != EndPoints.size() || StartPoints.size() != Colors.size())
        return;
    
    uint32 startIndex = static_cast<uint32>(LineBatchData->Vertices.size());
    
    // Reserve space for efficiency
    size_t lineCount = StartPoints.size();
    LineBatchData->Vertices.reserve(LineBatchData->Vertices.size() + lineCount * 2);
    LineBatchData->Color.reserve(LineBatchData->Color.size() + lineCount * 2);
    LineBatchData->Indices.reserve(LineBatchData->Indices.size() + lineCount * 2);
    
    // Add all lines at once
    for (size_t i = 0; i < lineCount; ++i)
    {
        uint32 currentIndex = startIndex + static_cast<uint32>(i * 2);
        
        // Add vertices
        LineBatchData->Vertices.push_back(StartPoints[i]);
        LineBatchData->Vertices.push_back(EndPoints[i]);
        
        // Add colors
        LineBatchData->Color.push_back(Colors[i]);
        LineBatchData->Color.push_back(Colors[i]);
        
        // Add indices for line (2 vertices per line)
        LineBatchData->Indices.push_back(currentIndex);
        LineBatchData->Indices.push_back(currentIndex + 1);
    }
}

void URenderer::AddLines(const TArray<FVector>& LineList, const FVector4& Color)
{
    if (!bLineBatchActive || !LineBatchData) return;

    // Validate input arrays have same size
    if (LineList.size() < 2)
        return;

    uint32 StartIdx = static_cast<uint32>(LineBatchData->Vertices.size());
    size_t AddLineCount = LineList.size();
    LineBatchData->Vertices.reserve(LineBatchData->Vertices.size() + AddLineCount);
    LineBatchData->Color.reserve(LineBatchData->Color.size() + AddLineCount);
    LineBatchData->Indices.reserve(LineBatchData->Indices.size() + AddLineCount);

    for (int i = 0; i < AddLineCount; i++)
    {
        uint32 currentIndex = StartIdx + static_cast<uint32>(i);

        // Add vertices
        LineBatchData->Vertices.push_back(LineList[i]);

        // Add colors
        LineBatchData->Color.push_back(Color);

        // Add indices for line (2 vertices per line)
        LineBatchData->Indices.push_back(currentIndex);
    }
}
void URenderer::EndLineBatch(const FMatrix& ModelMatrix, const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
{
    if (!bLineBatchActive || !LineBatchData || !DynamicLineMesh || LineBatchData->Vertices.empty())
    {
        bLineBatchActive = false;
        return;
    }
    
    // Efficiently update dynamic mesh data (no buffer recreation!)
    if (!DynamicLineMesh->UpdateData(LineBatchData, RHIDevice->GetDeviceContext()))
    {
        bLineBatchActive = false;
        return;
    }
    
    // Set up rendering state
    UpdateSetCBuffer(ModelBufferType(ModelMatrix));
    UpdateSetCBuffer(ViewProjBufferType(ViewMatrix, ProjectionMatrix));
    PrepareShader(LineShader);
    OMSetBlendState(true);
    
    // Render using dynamic mesh
    if (DynamicLineMesh->GetCurrentVertexCount() > 0 && DynamicLineMesh->GetCurrentIndexCount() > 0)
    {
        UINT stride = sizeof(FVertexSimple);
        UINT offset = 0;  
        
        ID3D11Buffer* vertexBuffer = DynamicLineMesh->GetVertexBuffer();
        ID3D11Buffer* indexBuffer = DynamicLineMesh->GetIndexBuffer();
        
        RHIDevice->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
        RHIDevice->GetDeviceContext()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
        RHIDevice->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
        RHIDevice->GetDeviceContext()->DrawIndexed(DynamicLineMesh->GetCurrentIndexCount(), 0, 0);
        
        // 라인 렌더링에 대한 DrawCall 통계 추가
        URenderingStatsCollector::GetInstance().IncrementDrawCalls();
    }
    
    bLineBatchActive = false;
}


UPrimitiveComponent* URenderer::GetCollidedPrimitive(int MouseX, int MouseY) const
{
    //GPU와 동기화 문제 때문에 Map이 호출될때까지 기다려야해서 피킹 하는 프레임에 엄청난 프레임 드랍이 일어남.
    //******비동기 방식으로 무조건 바꿔야함****************
    uint32 PickedId = 0;

    ID3D11DeviceContext* DeviceContext = RHIDevice->GetDeviceContext();
    //스테이징 버퍼를 가져와야 하는데 이걸 Device 추상 클래스가 Getter로 가지고 있는게 좋은 설계가 아닌 것 같아서 일단 캐스팅함
    D3D11RHI* RHI = static_cast<D3D11RHI*>(RHIDevice);

    D3D11_BOX Box{};
    Box.left = MouseX;
    Box.right= MouseX+1;
    Box.top = MouseY;
    Box.bottom = MouseY+1;
    Box.front = 0;
    Box.back = 1;
    
    DeviceContext->CopySubresourceRegion(
        RHI->GetIdStagingBuffer(),
        0,
        0, 0, 0,
        RHI->GetIdBuffer(),
        0,
        &Box);
    D3D11_MAPPED_SUBRESOURCE MapResource{};
    if (SUCCEEDED(DeviceContext->Map(RHI->GetIdStagingBuffer(), 0, D3D11_MAP_READ, 0, &MapResource)))
    {
        PickedId = *static_cast<uint32*>(MapResource.pData);
        DeviceContext->Unmap(RHI->GetIdStagingBuffer(), 0);
    }

    if (PickedId == 0)
        return nullptr;
    return Cast<UPrimitiveComponent>(GUObjectArray[PickedId]);
}


void URenderer::ResetRenderStateTracking()
{
    LastMaterial = nullptr;
    LastShader = nullptr;
    LastTexture = nullptr;
}

void URenderer::ClearLineBatch()
{
    if (!LineBatchData) return;
    
    LineBatchData->Vertices.clear();
    LineBatchData->Color.clear();
    LineBatchData->Indices.clear();
    
    bLineBatchActive = false;
}

