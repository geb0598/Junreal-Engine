#include "pch.h"
#include "SelectionManager.h"
#include "Picking.h"
#include "SceneLoader.h"
#include "CameraActor.h"
#include "StaticMeshActor.h"
#include "CameraComponent.h"
#include "ObjectFactory.h"
#include "TextRenderComponent.h"
#include "AABoundingBoxComponent.h"
#include "Mesh.h"

UWorld::UWorld() : ResourceManager(UResourceManager::GetInstance())
, UIManager(UUIManager::GetInstance())
, InputManager(UInputManager::GetInstance())
, SelectionManager(USelectionManager::GetInstance())
{
}


UWorld::~UWorld()
{
    for (AActor* Actor : Actors)
    {
        ObjectFactory::DeleteObject(Actor);
    }
    Actors.clear();

    // 카메라 정리
    ObjectFactory::DeleteObject(MainCameraActor);
    MainCameraActor = nullptr;

    // Grid 정리 
    ObjectFactory::DeleteObject(GridActor);
    GridActor = nullptr;
}

static void DebugRTTI_UObject(UObject* Obj, const char* Title)
{
    if (!Obj)
    {
        UE_LOG("[RTTI] Obj == null\r\n");
        return;
    }

    char buf[256];
    UE_LOG("========== RTTI CHECK ==========\r\n");
    if (Title)
    {
        std::snprintf(buf, sizeof(buf), "[RTTI] %s\r\n", Title);
        UE_LOG(buf);
    }

    // 1) 현재 동적 타입 이름
    std::snprintf(buf, sizeof(buf), "[RTTI] TypeName = %s\r\n", Obj->GetClass()->Name);
    UE_LOG(buf);

    // 2) IsA 체크 (파생 포함)
    std::snprintf(buf, sizeof(buf), "[RTTI] IsA<AActor>      = %d\r\n", (int)Obj->IsA<AActor>());
    UE_LOG(buf);
    std::snprintf(buf, sizeof(buf), "[RTTI] IsA<ACameraActor> = %d\r\n", (int)Obj->IsA<ACameraActor>());
    UE_LOG(buf);

    // 3) 정확한 타입 비교 (파생 제외)
    std::snprintf(buf, sizeof(buf), "[RTTI] EXACT ACameraActor = %d\r\n",
        (int)(Obj->GetClass() == ACameraActor::StaticClass()));
    UE_LOG(buf);

    // 4) 상속 체인 출력
    UE_LOG("[RTTI] Inheritance chain: ");
    for (const UClass* c = Obj->GetClass(); c; c = c->Super)
    {
        std::snprintf(buf, sizeof(buf), "%s%s", c->Name, c->Super ? " <- " : "\r\n");
        UE_LOG(buf);
    }
    //FString Name = Obj->GetName();
    std::snprintf(buf, sizeof(buf), "[RTTI] TypeName = %s\r\n", Obj->GetName().c_str());
    OutputDebugStringA(buf);
    OutputDebugStringA("================================\r\n");
}


void UWorld::Initialize()
{
    // === Scene 로딩 (임시) 및 Actor초기화  ===
    auto Primitives = FSceneLoader::Load("WorldData.Scene");
    for (auto Primitive : Primitives)
    {
        FString PrimitiveType = Primitive.Type + ".obj";

        AActor* Actor = NewObject<AStaticMeshActor>();
        Cast<AStaticMeshActor>(Actor)->GetStaticMeshComponent()->SetMeshResource(PrimitiveType);
        Cast<AStaticMeshActor>(Actor)->GetStaticMeshComponent()->SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
		if(PrimitiveType == "Sphere.obj")
            Cast<AStaticMeshActor>(Actor)->SetCollisionComponent(EPrimitiveType::Sphere);
        else
            Cast<AStaticMeshActor>(Actor)->SetCollisionComponent();
        //추후 변경 필요 !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        Actor->SetActorTransform(FTransform(Primitive.Location, FQuat::MakeFromEuler(Primitive.Rotation),
            Primitive.Scale));
        Actor->SetWorld(this);
        UTextRenderComponent* TextComp = NewObject<UTextRenderComponent>();
        TextComp->SetOwner(Actor);
        Actor->AddComponent(TextComp);
        Actor->SetWorld(this);
        TextComp->SetupAttachment( Actor->GetRootComponent());
//        Actors.push_back(Actor);
        Actors.push_back(Actor);
    }
    

	InitializeMainCamera();
    InitializeGrid();
	InitializeGizmo();
	
	// 액터 간 참조 설정
	SetupActorReferences();
}

void UWorld::InitializeMainCamera()
{
    MainCameraActor = NewObject<ACameraActor>();
    MainCameraActor->SetWorld(this);
    MainCameraActor->SetActorLocation({ 0, 0, -10 });

    DebugRTTI_UObject(MainCameraActor, "MainCameraActor");
    UIManager.SetCamera(MainCameraActor);

    EngineActors.Add(MainCameraActor);
}

void UWorld::InitializeGrid()
{
    GridActor = NewObject<AGridActor>();
    GridActor->SetActorTransform(FTransform(FVector{ 0, 0, 0 }, FQuat::MakeFromEuler(FVector{ 0, 0, 0 }),
        FVector{ 1, 1, 1 }));
    GridActor->Initialize();
    GridActor->SetWorld(this);

    // Add GridActor to Actors array so it gets rendered in the main loop
    EngineActors.push_back(GridActor);
    //EngineActors.push_back(GridActor);
}

void UWorld::InitializeGizmo()
{
    // === 기즈모 엑터 초기화 ===
    GizmoActor = NewObject<AGizmoActor>();
    GizmoActor->SetActorTransform(FTransform(FVector{ 0, 0, 0 }, FQuat::MakeFromEuler(FVector{ 0, -90, 0 }),
        FVector{ 1, 1, 1 }));
    GizmoActor->SetWorld(this);
    
    // 기즈모에 카메라 참조 설정
    if (MainCameraActor)
    {
        GizmoActor->SetCameraActor(MainCameraActor);
    }
    
    UIManager.SetGizmoActor(GizmoActor);
}

void UWorld::SetRenderer(URenderer* InRenderer)
{
    Renderer = InRenderer;
}

void UWorld::Render()
{
    FMatrix ViewMatrix = MainCameraActor->GetViewMatrix();
    FMatrix ProjectionMatrix = MainCameraActor->GetProjectionMatrix();
    FMatrix ModelMatrix;
    FVector rgb(1.0f, 1.0f, 1.0f);

    if (!Renderer) return;
    // === Begin Frame ===
    Renderer->BeginFrame();

    // === Begin Line Batch for all actors ===
    Renderer->BeginLineBatch();

    // === Draw Actors with Show Flag checks ===
    Renderer->SetViewModeType(ViewModeIndex);

    // 일반 액터들 렌더링 (Primitives Show Flag 체크)
    if (IsShowFlagEnabled(EEngineShowFlags::SF_Primitives))
    {
        for (AActor* Actor : Actors)
        {
            if (!Actor) continue;
            if (Actor->GetActorHiddenInGame()) continue;
            
            // StaticMesh Show Flag 체크
            if (Cast<AStaticMeshActor>(Actor) && !IsShowFlagEnabled(EEngineShowFlags::SF_StaticMeshes))
                continue;
                
            bool bIsSelected = SelectionManager.IsActorSelected(Actor);
            if (bIsSelected) {
                Renderer->OMSetDepthStencilState(EComparisonFunc::Always);
            }
            Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, 0, 0, 0, 0);
            
            for (USceneComponent* Component : Actor->GetComponents())
            {
                if (!Component) continue;

                if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
                {
                    if (!ActorComp->IsActive()) continue;
                }
                
                // Text Render Component Show Flag 체크
                if (Cast<UTextRenderComponent>(Component) && !IsShowFlagEnabled(EEngineShowFlags::SF_BillboardText))
                    continue;
                
                // Bounding Box Show Flag 체크  
                if (Cast<UAABoundingBoxComponent>(Component) && !IsShowFlagEnabled(EEngineShowFlags::SF_BoundingBoxes))
                    continue;
                
                if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
                {
                    Renderer->SetViewModeType(ViewModeIndex);
                    Primitive->Render(Renderer, ViewMatrix, ProjectionMatrix);
                    Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
                }
            }
            // 블랜드 스테이드 종료
            Renderer->OMSetBlendState(false);
        }
    }
    
    // Engine Actors (그리드 등) 렌더링
    for (AActor* EngineActor : EngineActors)
    {
        if (!EngineActor) continue;
        if (EngineActor->GetActorHiddenInGame()) continue;
        
        // Grid Show Flag 체크
        if (Cast<AGridActor>(EngineActor) && !IsShowFlagEnabled(EEngineShowFlags::SF_Grid))
            continue;
            
        for (USceneComponent* Component : EngineActor->GetComponents())
        {
            if (!Component) continue;

            if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
            {
                if (!ActorComp->IsActive()) continue;
            }
            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Renderer->SetViewModeType(ViewModeIndex);
                Primitive->Render(Renderer, ViewMatrix, ProjectionMatrix);
                Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
            }
        }
        // 블랜드 스테이드 종료
        Renderer->OMSetBlendState(false);
    }
    Renderer->EndLineBatch(FMatrix::Identity(), ViewMatrix, ProjectionMatrix);

    RenderGizmoActor();

    Renderer->UpdateHighLightConstantBuffer(false, rgb, 0, 0, 0, 0);
    UIManager.Render();
    // === End Frame ===
    Renderer->EndFrame();
}



void UWorld::Tick(float DeltaSeconds)
{
    //순서 바꾸면 안댐
    for (AActor* Actor : Actors)
    {
        if (Actor) Actor->Tick(DeltaSeconds);
    }
    for (AActor* EngineActor : EngineActors)
    {
        if (EngineActor) EngineActor->Tick(DeltaSeconds);
    }
    GizmoActor->Tick(DeltaSeconds);

    ProcessActorSelection();
    //Input Manager가 카메라 후에 업데이트 되어야함
    InputManager.Update();
    UIManager.Update(DeltaSeconds);
}

float UWorld::GetTimeSeconds() const
{
    return 0.0f;
}

FString UWorld::GenerateUniqueActorName(const FString& ActorType)
{
    // Get current count for this type
    int32& CurrentCount = ObjectTypeCounts[ActorType];
    FString UniqueName = ActorType + "_" + std::to_string(CurrentCount);
    CurrentCount++;
    return UniqueName;
}

//
// 액터 제거
//
bool UWorld::DestroyActor(AActor* Actor)
{
    if (!Actor)
    {
        return false; // nullptr 들어옴 → 실패
    }

    // SelectionManager에서 선택 해제 (메모리 해제 전에 하자)
    USelectionManager::GetInstance().DeselectActor(Actor);
    
    // UIManager에서 픽된 액터 정리
    if (UIManager.GetPickedActor() == Actor)
    {
        UIManager.ResetPickedActor();
    }

    // 배열에서 제거 시도
    auto it = std::find(Actors.begin(), Actors.end(), Actor);
    if (it != Actors.end())
    {
        Actors.erase(it);

        // 메모리 해제
        ObjectFactory::DeleteObject(Actor);
        
		// 삭제된 액터 정리
		USelectionManager::GetInstance().CleanupInvalidActors();
		
		return true; // 성공적으로 삭제
    }

    return false; // 월드에 없는 액터
}

inline FString ToObjFileName(const FString& TypeName)
{
    return TypeName + ".obj";
}

inline FString RemoveObjExtension(const FString& FileName)
{
    const FString Ext = ".obj";
    if (FileName.size() >= Ext.size() &&
        FileName.compare(FileName.size() - Ext.size(), Ext.size(), Ext) == 0)
    {
        return FileName.substr(0, FileName.size() - Ext.size());
    }
    return FileName;
}

void UWorld::CreateNewScene()
{
    // Safety: clear interactions that may hold stale pointers
    SelectionManager.ClearSelection();
    UIManager.ResetPickedActor();

    for (AActor* Actor : Actors)
    {
        ObjectFactory::DeleteObject(Actor);
    }
    Actors.Empty();
}

void UWorld::LoadScene(const FString& SceneName)
{
    // Start from a clean slate
    CreateNewScene();
    const TArray<FPrimitiveData>& Primitives = FSceneLoader::Load(SceneName + ".scene");

    for (const FPrimitiveData& Primitive : Primitives)
    {
        AStaticMeshActor* StaticMeshActor = SpawnActor<AStaticMeshActor>(
            FTransform(Primitive.Location,
                FQuat::MakeFromEuler(Primitive.Rotation),
                Primitive.Scale)
        );
        FString MeshType = ToObjFileName(Primitive.Type);
        StaticMeshActor->GetStaticMeshComponent()->SetMeshResource(ToObjFileName(Primitive.Type));
        StaticMeshActor->GetStaticMeshComponent()->SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
        if (MeshType == "Sphere.obj")
        {
            Cast<AStaticMeshActor>(StaticMeshActor)->SetCollisionComponent(EPrimitiveType::Sphere);
            //컬리젼 컴포넌트의 메쉬 정보를 강제로 세팅 
        }
        else
        {
            Cast<AStaticMeshActor>(StaticMeshActor)->SetCollisionComponent();
        }
    }
}

// 액터 인터페이스 관리 메소드들
void UWorld::SetupActorReferences()
{
    if (GizmoActor && MainCameraActor)
    {
        GizmoActor->SetCameraActor(MainCameraActor);
    }

}

void UWorld::ProcessActorSelection()
{
    if (!MainCameraActor) return;
    
    if (InputManager.IsMouseButtonPressed(LeftButton) && !InputManager.GetIsGizmoDragging())
    {
        if (AActor* PickedActor = CPickingSystem::PerformPicking(Actors, MainCameraActor))
        {
            SelectionManager.SelectActor(PickedActor);
            UIManager.SetPickedActor(PickedActor);
            if (GizmoActor)
            {
                GizmoActor->SetTargetActor(PickedActor);
                GizmoActor->SetActorLocation(PickedActor->GetActorLocation());
            }
        }
        else
        {
            UIManager.ResetPickedActor();
            SelectionManager.ClearSelection();
            if (GizmoActor)
            {
                GizmoActor->SetTargetActor(nullptr);
            }
        }
    }
}


void UWorld::SaveScene(const FString& SceneName)
{
    TArray<FPrimitiveData> Primitives;

    for (AActor* Actor : Actors)
    {
        FPrimitiveData Data;
        Data.Location = Actor->GetActorLocation();
        Data.Rotation = Actor->GetActorRotation().ToEuler();
        Data.Scale = Actor->GetActorScale();
        if (AStaticMeshActor* MeshActor = Cast<AStaticMeshActor>(Actor))
        {
            FString FilePath = MeshActor->GetStaticMeshComponent()->GetMeshResource()->GetFilePath();
            Data.Type = RemoveObjExtension(FilePath);
        }
        Primitives.push_back(Data);
    }
    FSceneLoader::Save(Primitives, SceneName);
}


void UWorld::RenderGizmoActor()
{
    if (!GizmoActor || !Renderer || !MainCameraActor) return;
    
    if (!SelectionManager.HasSelection()) return;
    
    FMatrix ViewMatrix = MainCameraActor->GetViewMatrix();
    FMatrix ProjectionMatrix = MainCameraActor->GetProjectionMatrix();
    FMatrix ModelMatrix;
    FVector rgb(1.0f, 1.0f, 1.0f);
    
    TArray<USceneComponent*>* Components = GizmoActor->GetGizmoComponents();
    if (!Components) return;
    
    for (uint32 i = 0; i < Components->Num(); ++i)
    {
        USceneComponent* Component = (*Components)[i];
        if (!Component) continue;
        
        // 컴포넌트 활성 상태 확인
        if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
        {
            if (!ActorComp->IsActive()) continue;
        }
        
        ModelMatrix = Component->GetWorldMatrix();
        Renderer->UpdateConstantBuffer(ModelMatrix, ViewMatrix, ProjectionMatrix);

        // 드래그 중이면 드래그하는 축만 하이라이트
        if (InputManager.GetIsGizmoDragging())
        {
            if (InputManager.GetDraggingAxis() == i + 1)
            {
                Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 1, 0, 1);
            }
            else
            {
                Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 0, 0, 1);
            }
        }
        // 드래그 중이 아니면 호버링 한 축만 하이라이트
        else if (CPickingSystem::IsHoveringGizmo(GizmoActor, MainCameraActor) == i + 1)
        {
            Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 1, 0, 1);
        }
        else
        {
            Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 0, 0, 1);
        }

        if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
        {
            Renderer->SetViewModeType(EViewModeIndex::VMI_Unlit);
            Renderer->OMSetDepthStencilState(EComparisonFunc::Always);
            Renderer->OMSetBlendState(true); // 필요 시

            Primitive->Render(Renderer, ViewMatrix, ProjectionMatrix);
            // 상태 복구
            Renderer->OMSetBlendState(false);
            Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
            Renderer->SetViewModeType(ViewModeIndex);
        }
    }
    Renderer->UpdateHighLightConstantBuffer(false, rgb, 0, 0, 0, 0);
    
    // 알파 블랜딩을 위한 blendstate
    Renderer->OMSetBlendState(true);
}
