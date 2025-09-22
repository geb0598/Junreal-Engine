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
#include "FViewport.h"
#include "SViewportWindow.h"
#include "SMultiViewportWindow.h"
#include "StaticMesh.h"
#include "ObjManager.h"

extern float CLIENTWIDTH;
extern float CLIENTHEIGHT;

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

    //// 3) 정확한 타입 비교 (파생 제외)
    //std::snprintf(buf, sizeof(buf), "[RTTI] EXACT ACameraActor = %d\r\n",
    //    (int)(Obj->GetClass() == ACameraActor::StaticClass()));
    //UE_LOG(buf);

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
	InitializeObjManager();

    // === Scene 로딩 (임시) 및 Actor초기화  ===
    auto Primitives = FSceneLoader::Load("WorldData.Scene");
    for (auto Primitive : Primitives)
    {
        FString PrimitiveType = "Data/" + Primitive.Type + ".obj";

        AActor* Actor = NewObject<AStaticMeshActor>();
        Cast<AStaticMeshActor>(Actor)->GetStaticMeshComponent()->SetStaticMesh(PrimitiveType);
        Cast<AStaticMeshActor>(Actor)->GetStaticMeshComponent()->SetMaterial("StaticMeshShader.hlsl", EVertexLayoutType::PositionColorTexturNormal);
		if(PrimitiveType == "Data/Sphere.obj")
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
    
    MainCameraActor = SpawnActor<ACameraActor>(FTransform());

    DebugRTTI_UObject(MainCameraActor, "MainCameraActor");
    UIManager.SetCamera(MainCameraActor);

    EngineActors.Add(MainCameraActor);
}

void UWorld::InitializeGrid()
{
    GridActor = SpawnActor<AGridActor>(FTransform());
    GridActor->Initialize();
  

    // Add GridActor to Actors array so it gets rendered in the main loop
    EngineActors.push_back(GridActor);
    //EngineActors.push_back(GridActor);
}

void UWorld::InitializeGizmo()
{
    // === 기즈모 엑터 초기화 ===
    GizmoActor = NewObject<AGizmoActor>();
    GizmoActor->SetWorld(this);
    GizmoActor->SetActorTransform(FTransform(FVector{ 0, 0, 0 }, FQuat::MakeFromEuler(FVector{ 0, -90, 0 }),
        FVector{ 1, 1, 1 }));
    // 기즈모에 카메라 참조 설정
    if (MainCameraActor)
    {
        GizmoActor->SetCameraActor(MainCameraActor);
    }
    
    UIManager.SetGizmoActor(GizmoActor);
}

void UWorld::InitializeObjManager()
{
	FObjManager::LoadObjStaticMesh("Data/Cube.obj");
	FObjManager::LoadObjStaticMesh("Data/Sphere.obj");
	FObjManager::LoadObjStaticMesh("Data/Triangle.obj");
	FObjManager::LoadObjStaticMesh("Data/Arrow.obj");
	FObjManager::LoadObjStaticMesh("Data/RotationHandle.obj");
    FObjManager::LoadObjStaticMesh("Data/ScaleHandle.obj");
	FObjManager::LoadObjStaticMesh("Data/car.obj");
	FObjManager::LoadObjStaticMesh("Data/cube-tex.obj");

    //FObjManager::LoadObjStaticMesh("Cube.obj");
    //FObjManager::LoadObjStaticMesh("Sphere.obj");
    //FObjManager::LoadObjStaticMesh("Triangle.obj");
    //FObjManager::LoadObjStaticMesh("Arrow.obj");
    //FObjManager::LoadObjStaticMesh("RotationHandle.obj");
    //FObjManager::LoadObjStaticMesh("ScaleHandle.obj");
    ////FObjManager::LoadObjStaticMesh("Data/car.obj");
    //FObjManager::LoadObjStaticMesh("cube-tex.obj");
}

void UWorld::SetRenderer(URenderer* InRenderer)
{
    Renderer = InRenderer;
}

void UWorld::Render()
{
    Renderer->BeginFrame();
    UIManager.Render();
 
    // UIManager의 뷰포트 전환 상태에 따라 렌더링 변경 SWidget으로 변경해줄거임
    if (UIManager.IsUsingMainViewport())
    {
        if (MainViewport)
        {
            MainViewport->OnRender();
        }
    }
    else
    {
        if (MultiViewport)
        {
            MultiViewport->OnRender();
        }
    }


    //프레임 종료 
    UIManager.EndFrame();
    Renderer->EndFrame();
}

void UWorld::RenderSingleViewport()
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

    

    Renderer->UpdateHighLightConstantBuffer(false, rgb, 0, 0, 0, 0);
    UIManager.Render();
    // === End Frame ===
    Renderer->EndFrame();
}

void UWorld::RenderViewports(ACameraActor* Camera, FViewport* Viewport)
{
    // 뷰포트의 실제 크기로 aspect ratio 계산
    float ViewportAspectRatio = static_cast<float>(Viewport->GetSizeX()) / static_cast<float>(Viewport->GetSizeY());
    if (Viewport->GetSizeY() == 0) ViewportAspectRatio = 1.0f; // 0으로 나누기 방지

    FMatrix ViewMatrix = Camera->GetViewMatrix();
    FMatrix ProjectionMatrix = Camera->GetProjectionMatrix(ViewportAspectRatio);
    if (!Renderer) return;
    FVector rgb(1.0f, 1.0f, 1.0f);

    // === Begin Line Batch for all actors ===
    Renderer->BeginLineBatch();

    // === Draw Actors with Show Flag checks ===
    Renderer->SetViewModeType(ViewModeIndex);

    // 일반 액터들 렌더링
    if (IsShowFlagEnabled(EEngineShowFlags::SF_Primitives))
    {
        for (AActor* Actor : Actors)
        {
            if (!Actor) continue;
            if (Actor->GetActorHiddenInGame()) continue;

            if (Cast<AStaticMeshActor>(Actor) && !IsShowFlagEnabled(EEngineShowFlags::SF_StaticMeshes))
                continue;

            bool bIsSelected = SelectionManager.IsActorSelected(Actor);
            if (bIsSelected)
                Renderer->OMSetDepthStencilState(EComparisonFunc::Always);

            Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, 0, 0, 0, 0);

            for (USceneComponent* Component : Actor->GetComponents())
            {
                if (!Component) continue;
                if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
                    if (!ActorComp->IsActive()) continue;

                if (Cast<UTextRenderComponent>(Component) && !IsShowFlagEnabled(EEngineShowFlags::SF_BillboardText))
                    continue;

                if (Cast<UAABoundingBoxComponent>(Component) && !IsShowFlagEnabled(EEngineShowFlags::SF_BoundingBoxes))
                    continue;

                if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
                {
                    Renderer->SetViewModeType(ViewModeIndex);
                    Primitive->Render(Renderer, ViewMatrix, ProjectionMatrix);
                    Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
                }
            }
            Renderer->OMSetBlendState(false);
        }
    }

    // 엔진 액터들 (그리드 등)
    for (AActor* EngineActor : EngineActors)
    {
        if (!EngineActor) continue;
        if (EngineActor->GetActorHiddenInGame()) continue;

        if (Cast<AGridActor>(EngineActor) && !IsShowFlagEnabled(EEngineShowFlags::SF_Grid))
            continue;

        for (USceneComponent* Component : EngineActor->GetComponents())
        {
            if (!Component) continue;
            if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
                if (!ActorComp->IsActive()) continue;

            if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
            {
                Renderer->SetViewModeType(ViewModeIndex);
                Primitive->Render(Renderer, ViewMatrix, ProjectionMatrix);
                Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
            }
        }
        Renderer->OMSetBlendState(false);
    }

    Renderer->EndLineBatch(FMatrix::Identity(), ViewMatrix, ProjectionMatrix);

  
    Renderer->UpdateHighLightConstantBuffer(false, rgb, 0, 0, 0, 0);
   
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
    ProcessViewportInput();
    //Input Manager가 카메라 후에 업데이트 되어야함
    InputManager.Update();
    UIManager.Update(DeltaSeconds);

    // 뷰포트 업데이트 - UIManager의 뷰포트 전환 상태에 따라
    if (UIManager.IsUsingMainViewport())
    {
        if (MainViewport)
        {
            MainViewport->SetRect(0, 0, CLIENTWIDTH, CLIENTHEIGHT);
            MainViewport->OnUpdate();
        }
    }
    else
    {
        if (MultiViewport)
        {
            MultiViewport->OnUpdate();
        }
    }
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
    return "Data/" + TypeName + ".obj";
}

inline FString RemoveObjExtension(const FString& FileName)
{
    const FString Extension = ".obj";

    // 마지막 경로 구분자 위치 탐색 (POSIX/Windows 모두 지원)
    const uint64 Sep = FileName.find_last_of("/\\");
    const uint64 Start = (Sep == FString::npos) ? 0 : Sep + 1;

    // 확장자 제거 위치 결정
    uint64 End = FileName.size();
    if (End >= Extension.size() &&
        FileName.compare(End - Extension.size(), Extension.size(), Extension) == 0)
    {
        End -= Extension.size();
    }

    // 베이스 이름(확장자 없는 파일명) 반환
    if (Start <= End)
        return FileName.substr(Start, End - Start);

    // 비정상 입력 시 원본 반환 (안전장치)
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



// 액터 인터페이스 관리 메소드들
void UWorld::SetupActorReferences()
{
    if (GizmoActor && MainCameraActor)
    {
        GizmoActor->SetCameraActor(MainCameraActor);
    }

}
//마우스 피킹관련 메소드
void UWorld::ProcessActorSelection()
{
    if (!MainCameraActor) return;

    if (InputManager.IsMouseButtonPressed(LeftButton) && !InputManager.GetIsGizmoDragging())
    {
        const FVector2D MousePosition = UInputManager::GetInstance().GetMousePosition();

        // UIManager의 뷰포트 전환 상태에 따라 마우스 처리
        if (UIManager.IsUsingMainViewport())
        {
            if (MainViewport)
            {
                MainViewport->OnMouseDown(MousePosition);
            }
        }
        else
        {
            if (MultiViewport)
            {
                MultiViewport->OnMouseDown(MousePosition);
            }
        }
    }
}
void UWorld::ProcessViewportInput()
{
    const FVector2D MousePosition = UInputManager::GetInstance().GetMousePosition();

    if (UIManager.IsUsingMainViewport())
    {
        if (MainViewport)
        {
            if (InputManager.IsMouseButtonPressed(LeftButton))
                MainViewport->OnMouseDown(MousePosition);
            if (InputManager.IsMouseButtonReleased(LeftButton))
                MainViewport->OnMouseUp(MousePosition);

            MainViewport->OnMouseMove(MousePosition);
        }
    }
    else
    {
        if (MultiViewport)
        {
            if (InputManager.IsMouseButtonPressed(LeftButton))
                MultiViewport->OnMouseDown(MousePosition);
            if (InputManager.IsMouseButtonReleased(LeftButton))
                MultiViewport->OnMouseUp(MousePosition);

            MultiViewport->OnMouseMove(MousePosition);
        }
    }
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
        FString ObjFileName = ToObjFileName(Primitive.Type);
        StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(ObjFileName);
        StaticMeshActor->GetStaticMeshComponent()->SetMaterial("Primitive.hlsl", EVertexLayoutType::PositionColor);
        if (ObjFileName == "Data/Sphere.obj")
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
            FString FilePath = MeshActor->GetStaticMeshComponent()->GetStaticMesh()->GetFilePath();
            Data.Type = RemoveObjExtension(FilePath);
        }
        Primitives.push_back(Data);
    }
    FSceneLoader::Save(Primitives, SceneName);
}

//
//void UWorld::RenderGizmoActor(const FMatrix& ViewMatrix, const FMatrix& ProjectionMatrix)
//{
//    if (!GizmoActor || !Renderer || !MainCameraActor) return;
//    
//    if (!SelectionManager.HasSelection()) return;
//  
//    FMatrix ModelMatrix;
//    FVector rgb(1.0f, 1.0f, 1.0f);
//    
//    TArray<USceneComponent*>* Components = GizmoActor->GetGizmoComponents();
//    if (!Components) return;
//    
//    for (uint32 i = 0; i < Components->Num(); ++i)
//    {
//        USceneComponent* Component = (*Components)[i];
//        if (!Component) continue;
//        
//        // 컴포넌트 활성 상태 확인
//        if (UActorComponent* ActorComp = Cast<UActorComponent>(Component))
//        {
//            if (!ActorComp->IsActive()) continue;
//        }
//        
//        ModelMatrix = Component->GetWorldMatrix();
//        Renderer->UpdateConstantBuffer(ModelMatrix, ViewMatrix, ProjectionMatrix);
//
//        // 드래그 중이면 드래그하는 축만 하이라이트
//        if (InputManager.GetIsGizmoDragging())
//        {
//            if (InputManager.GetDraggingAxis() == i + 1)
//            {
//                Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 1, 0, 1);
//            }
//            else
//            {
//                Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 0, 0, 1);
//            }
//        }
//        // 드래그 중이 아니면 호버링 한 축만 하이라이트
//        else if (CPickingSystem::IsHoveringGizmo(GizmoActor, MainCameraActor) == i + 1)
//        {
//            Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 1, 0, 1);
//        }
//        else
//        {
//            Renderer->UpdateHighLightConstantBuffer(true, rgb, i + 1, 0, 0, 1);
//        }
//
//        if (UPrimitiveComponent* Primitive = Cast<UPrimitiveComponent>(Component))
//        {
//            Renderer->SetViewModeType(EViewModeIndex::VMI_Unlit);
//            Renderer->OMSetDepthStencilState(EComparisonFunc::Always);
//            Renderer->OMSetBlendState(true); // 필요 시
//
//            Primitive->Render(Renderer, ViewMatrix, ProjectionMatrix);
//            // 상태 복구
//            Renderer->OMSetBlendState(false);
//            Renderer->OMSetDepthStencilState(EComparisonFunc::LessEqual);
//            Renderer->SetViewModeType(ViewModeIndex);
//        }
//    }
//    Renderer->UpdateHighLightConstantBuffer(false, rgb, 0, 0, 0, 0);
//    
//    // 알파 블랜딩을 위한 blendstate
//    Renderer->OMSetBlendState(true);
//}

AGizmoActor* UWorld::GetGizmoActor()
{
    return GizmoActor;
}
