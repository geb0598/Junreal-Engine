#include "World.h"
#include "Actor.h"
#include "ResourceManager.h"
#include "UI/UIManager.h"
#include "InputManager.h"
#include "SelectionManager.h"
#include "Picking.h"
#include "Renderer.h"
#include "SceneLoader.h"
#include "CameraActor.h"
#include "StaticMeshActor.h"
#include "CameraComponent.h"
#include "Vector.h"
#include "UI/GlobalConsole.h"
#include "ObjectFactory.h"
#include "GridComponent.h"


UWorld::UWorld() : ResourceManager(UResourceManager::GetInstance())
                   , UIManager(UUIManager::GetInstance())
                   , InputManager(UInputManager::GetInstance())
                   , SelectionManager(USelectionManager::GetInstance())
{
    GridActor = NewObject<AGridActor>();
    GridActor->SetActorTransform(FTransform(FVector{0, 0, 0}, FQuat::MakeFromEuler(FVector{0, 0, 0}),
                                            FVector{1, 1, 1}));
    GridActor->SetWorld(this);
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

static void DebugRTTI_UObject(const UObject* Obj, const char* Title)
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
    UE_LOG("================================\r\n");
}


void UWorld::Initialize()
{
    // === Scene 로딩 (임시) 및 Actor초기화  ===
    auto Primitives = FSceneLoader::Load("WorldData.Scene");
    for (auto Primitive : Primitives)
    {
        FString PrimitiveType = "Cube.obj";
        if (Primitive.Type == "Cube")
            PrimitiveType = "Cube.obj";
        else if (Primitive.Type == "Sphere")
            PrimitiveType = "Sphere.obj";
        else if (Primitive.Type == "Triangle")
            PrimitiveType = "Triangle.obj";
        else if (Primitive.Type == "Arrow")
            PrimitiveType = "Arrow.obj";

        AActor* Actor = NewObject<AStaticMeshActor>();
        Cast<AStaticMeshActor>(Actor)->GetStaticMeshComponent()->SetStaticMesh(PrimitiveType);

        Actor->SetActorTransform(FTransform(Primitive.Location, FQuat::MakeFromEuler(Primitive.Rotation),
                                            Primitive.Scale));
        Actor->SetWorld(this);
        Actors.push_back(Actor);
    }


    GizmoActor = NewObject<AGizmoActor>();
    GizmoActor->SetActorTransform(FTransform(FVector{0, 0, 0}, FQuat::MakeFromEuler(FVector{0, -90, 0}),
                                             FVector{1, 1, 1}));
    GizmoActor->SetWorld(this);
    UIManager.SetGizmoActor(GizmoActor);
    //AActor* GridActor = new AGridActor();

    //Actors.push_back(GridActor);


    // === 카메라 엑터 초기화 ===
    // 카메라
    MainCameraActor = NewObject<ACameraActor>();
    MainCameraActor->SetWorld(this);
    MainCameraActor->SetActorLocation({0, 0, -10});

    DebugRTTI_UObject(MainCameraActor, "MainCameraActor");
    UIManager.SetCamera(MainCameraActor);

    ResourceManager.CreatePrimitiveShader();
}

void UWorld::SetRenderer(URenderer* InRenderer)
{
    Renderer = InRenderer;
}

// 순수 누적 방식의 카메라 회전 상태 (전역)
float CameraYawDeg = 0.0f; // 월드 Up(Y) 기준 Yaw (무제한 누적)
float CameraPitchDeg = 0.0f; // 로컬 Right 기준 Pitch (제한됨)

void UWorld::Render()
{
    FMatrix ViewMatrix = MainCameraActor->GetViewMatrix();
    FMatrix ProjectionMatrix = MainCameraActor->GetProjectionMatrix();
    FMatrix ModelMatrix;
    FVector rgb(1.0f, 1.0f, 1.0f);


    if (!Renderer) return;
    // === Begin Frame ===
    Renderer->BeginFrame();


    // === Draw Grid ===


    // === Draw Primitive ===
    //Renderer->PrepareShader();
    Renderer->PrepareShader(ResourceManager.GetPrimitiveShader());


    for (USceneComponent* Comp : GridActor->GetComponents())
    {
        ModelMatrix = Comp->GetWorldMatrix();
        Renderer->UpdateConstantBuffer(ModelMatrix, ViewMatrix, ProjectionMatrix);
        if (UStaticMeshComponent* Prim = dynamic_cast<UStaticMeshComponent*>(Comp))
        {
            Renderer->DrawIndexedPrimitiveComponent(Prim);
        }
    }

    for (AActor* Actor : Actors)
    {
        if (!Actor) continue;

        bool bIsSelected = SelectionManager.IsActorSelected(Actor);
        Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, 0, 0, 0, 0);
        // UE_LOG("bIsSelected %d", bIsSelected);

        if (bIsSelected)
        {
            TArray<USceneComponent*>* Components = GizmoActor->GetGizmoComponents();

            EGizmoMode GizmoMode = GizmoActor->GetGizmoMode();

            for (int32 i = 0; i < Components->Num(); ++i)
            {
                ModelMatrix = (*Components)[i]->GetWorldMatrix();
                Renderer->UpdateConstantBuffer(ModelMatrix, ViewMatrix, ProjectionMatrix);

                // 드래그 중이면 드래그하는 축만
                if (InputManager.GetIsGizmoDragging())
                {
                    if (InputManager.GetDraggingAxis() == i + 1)
                    {
                        Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, i + 1, 1, 0, 1);
                    }
                    else
                    {
                        Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, i + 1, 0, 0, 1);
                    }
                }
                // 아니면 호버링 한 축만
                else if (CPickingSystem::IsHoveringGizmo(GizmoActor, MainCameraActor) == i + 1)
                {
                    Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, i + 1, 1, 0, 1);
                }
                else
                {
                    Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, i + 1, 0, 0, 1);
                }

                if (UStaticMeshComponent* Primitive = Cast<UStaticMeshComponent>((*Components)[i]))
                {
                    Renderer->DrawIndexedPrimitiveComponent(Primitive);
                }
            }
            Renderer->UpdateHighLightConstantBuffer(bIsSelected, rgb, 0, 0, 0, 0);

            // 알파 블랜딩을 위한 blendstate
            Renderer->OMSetBlendState(true);
        }


        // 액터의 모든 컴포넌트 순회
        for (USceneComponent* Component : Actor->GetComponents())
        {
            if (!Component) continue;
            //  
            FMatrix ModelMatrix = Component->GetWorldMatrix();
            Renderer->UpdateConstantBuffer(ModelMatrix, ViewMatrix, ProjectionMatrix);

            // 컴포넌트가 StaticMesh를 가진 경우만 Draw
            if (UStaticMeshComponent* Primitive = Cast<UStaticMeshComponent>(Component))
            {
                Renderer->DrawIndexedPrimitiveComponent(Primitive);
            }
        }

        // 블랜드 스테이드 종료
        Renderer->OMSetBlendState(false);
    }

    Renderer->UpdateHighLightConstantBuffer(false, rgb, 0, 0, 0, 0);

    UIManager.Render();


    // === End Frame ===
    Renderer->EndFrame();
}


void UWorld::Tick(float DeltaSeconds)
{
    // 액터 로직 업데이트
    for (AActor* Actor : Actors)
    {
        if (Actor) Actor->Tick(DeltaSeconds);
    }

    // 기즈모 드래그 상태 관리를 위한 정적 변수들
    // static bool bIsDragging = false;
    // static uint32 DraggingAxis = 0;

    if (MainCameraActor)
    {
        // 선택된 액터가 있을 때 기즈모 호버링을 위한 상시 피킹 추가 가동
        if (SelectionManager.HasSelection())
        {
            //기즈모 위치조정
            GizmoActor->SetActorLocation(SelectionManager.GetSelectedActor()->GetActorLocation());

            // 어느 기즈모가 호버링 됐는지 출력 return 1 or 2 or 3
            uint32 HoveringResult = CPickingSystem::IsHoveringGizmo(GizmoActor, MainCameraActor);

            // 드래그 시작 감지
            if (!InputManager.GetIsGizmoDragging() && HoveringResult > 0 && InputManager.IsMouseButtonPressed(
                LeftButton))
            {
                InputManager.SetIsGizmoDragging(true);
                InputManager.SetDraggingAxis(HoveringResult);
                UE_LOG("Start dragging axis: %d", InputManager.GetDraggingAxis());
            }

            //여기서 모드 가져옴
            EGizmoMode GizmoMode = GizmoActor->GetGizmoMode();

            // 드래그 중일 때 액터 이동
            if (InputManager.GetIsGizmoDragging() && InputManager.IsMouseButtonDown(LeftButton))
            {
                FVector2D MouseDelta = InputManager.GetMouseDelta();
                if ((MouseDelta.X * MouseDelta.X + MouseDelta.Y * MouseDelta.Y) > 0.0f)
                {
                    //AActor* SelectedActor = SelectionManager.GetSelectedActor();

                    // 모드에 따라서 Picked Actor 행동 결정

                    AActor* SelectedActor = SelectionManager.GetSelectedActor();
                    CPickingSystem::DragActorWithGizmo(SelectedActor, GizmoActor, InputManager.GetDraggingAxis(), MouseDelta, MainCameraActor, GizmoMode);

                    // 기즈모 위치도 함께 업데이트
                    GizmoActor->SetActorLocation(SelectedActor->GetActorLocation());

                    UE_LOG("Dragging axis %d, delta: (%.3f, %.3f)", InputManager.GetDraggingAxis(), MouseDelta.X,
                           MouseDelta.Y);
                }
            }

            // 드래그 종료 감지
            if (InputManager.GetIsGizmoDragging() && InputManager.IsMouseButtonReleased(LeftButton))
            {
                InputManager.SetIsGizmoDragging(false);
                InputManager.SetDraggingAxis(0);
                UE_LOG("Stop dragging");
            }
            if (InputManager.IsKeyPressed(VK_SPACE))
            {
                int GizmoModeIndex = static_cast<int>(GizmoActor->GetMode());
                GizmoModeIndex = (GizmoModeIndex + 1) % 3;  // 3 = enum 개수
                EGizmoMode GizmoMode = static_cast<EGizmoMode>(GizmoModeIndex);
                GizmoActor->NextMode(GizmoMode);
            }
        }

        // 좌클릭으로 피킹 (기즈모 드래그 중이 아닐 때만)
        if (InputManager.IsMouseButtonPressed(LeftButton) && !InputManager.GetIsGizmoDragging())
        {
            // PickingSystem을 사용한 피킹 처리
            if (AActor* PickedActor = CPickingSystem::PerformPicking(Actors, MainCameraActor))
            {
                UIManager.SetPickedActor(PickedActor);
                // 색 강조를 위한 플래그

                // SelectionManager를 통해 액터 선택
                SelectionManager.SelectActor(PickedActor);

                GizmoActor->SetActorLocation(PickedActor->GetActorLocation());
            }
            else
            {
                //TUUIManager.GetPickedActor()->ClearPickedFlag();

                UIManager.ResetPickedActor();
                // 선택 해제
                SelectionManager.ClearSelection();
            }
        }

        // 에디터 카메라 조작
        ProcessEditorCameraInput(DeltaSeconds);

        // 드래그 종료시
        if (InputManager.IsMouseButtonReleased(RightButton))
        {
            FVector UICameraDeg = UIManager.GetTempCameraRotation();
            CameraYawDeg = UICameraDeg.Y;
            CameraPitchDeg = UICameraDeg.X;
        }
        //UE_LOG("Real CAMERA ROTATION    Pitch : %f  Yaw : %f", CameraPitchDeg, CameraYawDeg);
    }
	InputManager.Update();
	UIManager.Update(DeltaSeconds);


    // for (int i = 0; i < 1000; i++)
    // {
    //     FTransform SpawnActorTF(
    //         FVector(
    //             (static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f,
    //             0,
    //             (static_cast<float>(rand()) / RAND_MAX) * 10.0f - 5.0f
    //         ),
    //         FQuat::MakeFromEuler({ 0, 0, 0 }),
    //         FVector(
    //             static_cast<float>(rand()) / RAND_MAX * 2.0f + 1.0f,
    //             static_cast<float>(rand()) / RAND_MAX * 2.0f + 1.0f,
    //             static_cast<float>(rand()) / RAND_MAX * 2.0f + 1.0f
    //         )
    //     );
    //
    //     AStaticMeshActor* SpawnedActor = SpawnActor<AStaticMeshActor>(SpawnActorTF);
    //     //여기에 타입셋하는 코드 추가
    //     int32 Random = rand() % 3;
    //     const char* Mesh =
    //         (Random == 0) ? "Sphere.obj" :
    //         (Random == 1) ? "Cube.obj" :
    //         "Triangle.obj";
    //     SpawnedActor->GetStaticMeshComponent()->SetStaticMesh(Mesh);
    // }
}

float UWorld::GetTimeSeconds() const
{
    return 0.0f;
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

    // 배열에서 제거 시도
    auto it = std::find(Actors.begin(), Actors.end(), Actor);
    if (it != Actors.end())
    {
        Actors.erase(it);

        // 메모리 해제
        ObjectFactory::DeleteObject(Actor);
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
        StaticMeshActor->GetStaticMeshComponent()->SetStaticMesh(ToObjFileName(Primitive.Type));
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
// 에디터 카메라 조작 구현
//
void UWorld::ProcessEditorCameraInput(float DeltaSeconds)
{
    if (!MainCameraActor) return;

    // 우클릭 드래그로 카메라 회전 및 이동
    if (InputManager.IsMouseButtonDown(RightButton))
    {
        ProcessCameraRotation(DeltaSeconds);
        ProcessCameraMovement(DeltaSeconds);
    }
    //else
    //{
    //    FVector UICameraDeg = UIManager.GetTempCamerarotation();
    //    CameraYawDeg = UICameraDeg.Y;
    //    CameraPitchDeg = UICameraDeg.X;
    //}
}


static inline float Clamp(float v, float a, float b) { return v < a ? a : (v > b ? b : v); }

void UWorld::ProcessCameraRotation(float DeltaSeconds)
{
    if (!MainCameraActor) return;

    
    //FVector UICameraDeg = UIManager.GetTempCamerarotation();
    //CameraYawDeg += UICameraDeg.Y; // 월드 Up(Y) 기준 Yaw (무제한 누적)
    //CameraPitchDeg += UICameraDeg.X; // 로컬 Right 기준 Pitch (제한됨)


    FVector2D MouseDelta = InputManager.GetMouseDelta();
    if (MouseDelta.X == 0.0f && MouseDelta.Y == 0.0f) return;

    // 1) Pitch/Yaw만 누적 (Roll은 UIManager에서 관리하는 값으로 고정)
    CameraYawDeg += MouseDelta.X * MouseSensitivity;
    CameraPitchDeg += MouseDelta.Y * MouseSensitivity;

    // 각도 정규화 및 Pitch 제한
    CameraYawDeg = NormalizeAngleDeg(CameraYawDeg); // -180 ~ 180 범위로 정규화
    CameraPitchDeg = Clamp(CameraPitchDeg, -89.0f, 89.0f); // Pitch는 짐벌락 방지를 위해 제한

    // 2) UIManager의 저장된 Roll 값을 가져와서 축별 쿼터니언 합성
    float CurrentRoll = UIManager.GetStoredRoll();

    // 축별 개별 쿼터니언 생성
    FQuat PitchQuat = FQuat::FromAxisAngle(FVector(0, 1, 0), DegreeToRadian(CameraPitchDeg));
    FQuat YawQuat = FQuat::FromAxisAngle(FVector(0, 0, 1), DegreeToRadian(CameraYawDeg));
    FQuat RollQuat = FQuat::FromAxisAngle(FVector(1, 0, 0), DegreeToRadian(CurrentRoll));

    // RzRxRy 순서로 회전 합성 (Roll(Z) → Pitch(X) → Yaw(Y))
    FQuat FinalRotation = YawQuat * PitchQuat * RollQuat;
    FinalRotation.Normalize();

    MainCameraActor->SetActorRotation(FinalRotation);

    // 3) UIManager에 마우스로 변경된 Pitch/Yaw 값 동기화
    UIManager.UpdateMouseRotation(CameraPitchDeg, CameraYawDeg);
}


static inline FVector RotateByQuat(const FVector& Vector, const FQuat& Quat)
{
    return Quat.RotateVector(Vector);
}

void UWorld::ProcessCameraMovement(float DeltaSeconds)
{
    if (!MainCameraActor) return;

    FVector Move(0, 0, 0);

    // 1) 카메라 회전(쿼터니언)에서 로컬 기저 추출 (스케일 영향 제거)
    const FQuat Quat = MainCameraActor->GetActorRotation(); // (x,y,z,w)
    // DirectX LH 기준: Right=+X, Up=+Y, Forward=+Z
    const FVector Right = Quat.RotateVector(FVector(0, 1, 0)).GetNormalized();
    const FVector Up = Quat.RotateVector(FVector(0, 0, 1)).GetNormalized();
    const FVector Forward = Quat.RotateVector(FVector(1, 0, 0)).GetNormalized();

    // 2) 입력 누적 (WASD + QE)
    if (InputManager.IsKeyDown('W')) Move += Forward;
    if (InputManager.IsKeyDown('S')) Move -= Forward;
    if (InputManager.IsKeyDown('D')) Move += Right;
    if (InputManager.IsKeyDown('A')) Move -= Right;
    if (InputManager.IsKeyDown('E')) Move += Up;
    if (InputManager.IsKeyDown('Q')) Move -= Up;

    // 3) 이동 적용
    if (Move.SizeSquared() > 0.0f)
    {
        const float speed = CameraMoveSpeed * DeltaSeconds * 2.5f;
        Move = Move.GetNormalized() * speed;

        const FVector P = MainCameraActor->GetActorLocation();
        MainCameraActor->SetActorLocation(P + Move);
    }
}
