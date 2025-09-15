#include "pch.h"
#include "GizmoActor.h"
#include "GizmoArrowComponent.h"
#include "GizmoScaleComponent.h"
#include "GizmoRotateComponent.h"
#include "CameraActor.h"
#include "SelectionManager.h"
#include "InputManager.h"
#include "UI/UIManager.h"
#include "Picking.h"

AGizmoActor::AGizmoActor()
{
    Name = "Gizmo Actor";

    //======= Arrow Component 생성 =======
    ArrowX = NewObject<UGizmoArrowComponent>();
    ArrowY = NewObject<UGizmoArrowComponent>();
    ArrowZ = NewObject<UGizmoArrowComponent>();

    ArrowX->SetDirection(FVector(1.0f, 0.0f, 0.0f));//빨 
    ArrowY->SetDirection(FVector(0.0f, 1.0f, 0.0f));//초
    ArrowZ->SetDirection(FVector(0.0f, 0.0f, 1.0f));//파

    ArrowX->SetColor(FVector(1.0f, 0.0f, 0.0f));
    ArrowY->SetColor(FVector(0.0f, 1.0f, 0.0f));
    ArrowZ->SetColor(FVector(0.0f, 0.0f, 1.0f));

    ArrowX->SetupAttachment(RootComponent);
    ArrowY->SetupAttachment(RootComponent);
    ArrowZ->SetupAttachment(RootComponent);

    ArrowX->SetRelativeScale({1, 1, 3});
    ArrowY->SetRelativeScale({1, 1, 3});
    ArrowZ->SetRelativeScale({1, 1, 3});

    if (ArrowX)  ArrowX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -180))); // X축//빨
    if (ArrowY) ArrowY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90)));//초
    if (ArrowZ)  ArrowZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // Z축//파


    AddComponent(ArrowX);
    AddComponent(ArrowY);
    AddComponent(ArrowZ);
    GizmoArrowComponents.Add(ArrowX);
    GizmoArrowComponents.Add(ArrowY);   
    GizmoArrowComponents.Add(ArrowZ);
 //   ArrowX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(-90, 0, 0))); // X축
//    ArrowZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90))); // Z축

    //======= Rotate Component 생성 =======
    RotateX = NewObject<UGizmoRotateComponent>();
    RotateY = NewObject<UGizmoRotateComponent>();
    RotateZ = NewObject<UGizmoRotateComponent>();

    RotateX->SetDirection(FVector(1.0f, 0.0f, 0.0f));
    RotateY->SetDirection(FVector(0.0f, 1.0f, 0.0f));
    RotateZ->SetDirection(FVector(0.0f, 0.0f, 1.0f));

    RotateX->SetColor(FVector(1.0f, 0.0f, 0.0f));
    RotateY->SetColor(FVector(0.0f, 1.0f, 0.0f));
    RotateZ->SetColor(FVector(0.0f, 0.0f, 1.0f));

    RotateX->SetupAttachment(RootComponent);
    RotateY->SetupAttachment(RootComponent);
    RotateZ->SetupAttachment(RootComponent);

    RotateX->SetRelativeScale({ 0.02, 0.02, 0.02 });
    RotateY->SetRelativeScale({ 0.02, 0.02, 0.02 });
    RotateZ->SetRelativeScale({ 0.02, 0.02, 0.02 });

    AddComponent(RotateX);
    AddComponent(RotateY);
    AddComponent(RotateZ);
    GizmoRotateComponents.Add(RotateX);
    GizmoRotateComponents.Add(RotateY);
    GizmoRotateComponents.Add(RotateZ);

    if (RotateX)         RotateX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90))); // X축
    if (RotateY)         RotateY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // Z축

    if (RotateZ)          RotateZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));

    //======= Scale Component 생성 =======
    ScaleX = NewObject<UGizmoScaleComponent>();
    ScaleY = NewObject<UGizmoScaleComponent>();
    ScaleZ = NewObject<UGizmoScaleComponent>();

    ScaleX->SetDirection(FVector(1.0f, 0.0f, 0.0f));
    ScaleY->SetDirection(FVector(0.0f, 1.0f, 0.0f));
    ScaleZ->SetDirection(FVector(0.0f, 0.0f, 1.0f));

    ScaleX->SetColor(FVector(1.0f, 0.0f, 0.0f));
    ScaleY->SetColor(FVector(0.0f, 1.0f, 0.0f));
    ScaleZ->SetColor(FVector(0.0f, 0.0f, 1.0f));

    ScaleX->SetupAttachment(RootComponent);
    ScaleY->SetupAttachment(RootComponent);
    ScaleZ->SetupAttachment(RootComponent);

    ScaleX->SetRelativeScale({ 0.02, 0.02, 0.02 });
    ScaleY->SetRelativeScale({ 0.02, 0.02, 0.02 });
    ScaleZ->SetRelativeScale({ 0.02, 0.02, 0.02 });

    if (ScaleX) ScaleX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90))); // X축//빨
    if (ScaleY) ScaleY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 00))); // Z축//초
    if (ScaleZ) ScaleZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));//파

    AddComponent(ScaleX);
    AddComponent(ScaleY);
    AddComponent(ScaleZ);
    GizmoScaleComponents.Add(ScaleX);
    GizmoScaleComponents.Add(ScaleY);
    GizmoScaleComponents.Add(ScaleZ);
    ScaleX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // X축
    ScaleY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90))); // Z축

    CurrentMode = EGizmoMode::Translate;
    
    // 매니저 참조 초기화
    SelectionManager = &USelectionManager::GetInstance();
    InputManager = &UInputManager::GetInstance();
    UIManager = &UUIManager::GetInstance();
}

void AGizmoActor::Tick(float DeltaSeconds)
{
    if (!SelectionManager) SelectionManager = &USelectionManager::GetInstance();
    if (!InputManager) InputManager = &UInputManager::GetInstance();
    if (!UIManager) UIManager = &UUIManager::GetInstance();
    
    // 컴포넌트 활성화 상태 업데이트    
    if (SelectionManager->HasSelection() && CameraActor)
    {
        TargetActor = SelectionManager->GetSelectedActor();
        
        // 기즈모 위치를 선택된 액터 위치로 업데이트
        if (TargetActor)
        {
            SetSpaceWorldMatrix(CurrentSpace, TargetActor);
            SetActorLocation(TargetActor->GetActorLocation());
        }
        
        ProcessGizmoInteraction(DeltaSeconds);
    }
    else
    {
        TargetActor = nullptr;
    }
    UpdateComponentVisibility();
}

AGizmoActor::~AGizmoActor()
{
    // Components are centrally destroyed by AActor's destructor
    ArrowX = ArrowY = ArrowZ = nullptr;
    ScaleX = ScaleY = ScaleZ = nullptr;
    RotateX = RotateY = RotateZ = nullptr;
}

void AGizmoActor::SetMode(EGizmoMode NewMode)
{
    CurrentMode = NewMode;
}
EGizmoMode  AGizmoActor::GetMode()
{
    return CurrentMode;
}

void AGizmoActor::SetSpaceWorldMatrix(EGizmoSpace NewSpace,AActor* PickedActor)
{
    SetSpace(NewSpace);

    if (NewSpace == EGizmoSpace::World)
    {
       
        // 월드 고정 → 기즈모 축은 항상 X/Y/Z
           // 월드 고정 → 기즈모 축은 항상 X/Y/Z
        if (ArrowX)  ArrowX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -180))); // X축//빨
        if (ArrowY) ArrowY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90)));//초
        if (ArrowZ)  ArrowZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // Z축//파


        if (ScaleX) ScaleX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90))); // X축//빨
        if (ScaleY) ScaleY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 00))); // Z축//초
        if (ScaleZ) ScaleZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));//파


        if (RotateX)         RotateX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90))); // X축
        if (RotateY)         RotateY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // Z축

        if (RotateZ)          RotateZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));
    
    }
    else if (NewSpace == EGizmoSpace::Local)
    {
        if (!PickedActor)
        return;

    // 타겟 액터 회전 가져오기
    FQuat TargetRot = PickedActor->GetActorRotation();
   // FQuat AC = FQuat::MakeFromEuler(FVector(, 0, 0));

    // ───────── Translate Gizmo ─────────
   // ArrowX->AddRelativeRotation(AC);
       // 월드 고정 → 기즈모 축은 항상 X/Y/Z
    if (ArrowX)  ArrowX->SetRelativeRotation(TargetRot*FQuat::MakeFromEuler(FVector(0, 0, -180))); // X축//빨
    if (ArrowY) ArrowY->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, -90)));//초
    if (ArrowZ)  ArrowZ->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 90, 0))); // Z축//파


    if (ScaleX) ScaleX->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, -90))); // X축//빨
    if (ScaleY) ScaleY->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, 00))); // Z축//초
    if (ScaleZ) ScaleZ->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(90, 0, 0)));//파


    if (RotateX)         RotateX->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, 90))); // X축
    if (RotateY)         RotateY->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 90, 0))); // Z축

    if (RotateZ)          RotateZ->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(90, 0, 0)));

   
    }
    
}


void AGizmoActor::NextMode(EGizmoMode GizmoMode)
{
    CurrentMode = GizmoMode;
}

TArray<USceneComponent*>* AGizmoActor::GetGizmoComponents()
{
    switch (CurrentMode)
    {
    case EGizmoMode::Translate:
        return &GizmoArrowComponents;
    case EGizmoMode::Rotate:
        return &GizmoRotateComponents;
    case EGizmoMode::Scale:
        return &GizmoScaleComponents;
    }
    return nullptr;
}

EGizmoMode AGizmoActor::GetGizmoMode() const
{
    return CurrentMode;
}

// 개선된 축 투영 함수 - 수직 각도에서도 안정적
static FVector2D GetStableAxisDirection(const FVector& WorldAxis, const ACameraActor* Camera)
{
    const FVector CameraRight = Camera->GetRight();
    const FVector CameraUp = Camera->GetUp();
    const FVector CameraForward = Camera->GetForward();
    
    // 축과 카메라 forward의 각도 확인 (수직도 측정)
    float ForwardDot = FVector::Dot(WorldAxis.GetNormalized(), CameraForward.GetNormalized());
    float PerpendicularThreshold = 0.95f; // cos(약 18도)
    
    // 거의 수직인 경우 (축이 스크린과 거의 평행)
    if (std::fabs(ForwardDot) > PerpendicularThreshold)
    {
        // 가장 큰 성분을 가진 카메라 축 성분 사용
        float RightComponent = std::fabs(FVector::Dot(WorldAxis, CameraRight));
        float UpComponent = std::fabs(FVector::Dot(WorldAxis, CameraUp));
        
        if (RightComponent > UpComponent)
        {
            // Right 성분이 더 클 때: X축 우선
            float Sign = FVector::Dot(WorldAxis, CameraRight) > 0 ? 1.0f : -1.0f;
            return FVector2D(Sign, 0.0f);
        }
        else
        {
            // Up 성분이 더 클 때: Y축 우선 (DirectX는 Y가 아래쪽이므로 반전)
            float Sign = FVector::Dot(WorldAxis, CameraUp) > 0 ? -1.0f : 1.0f;
            return FVector2D(0.0f, Sign);
        }
    }
    
    // 일반적인 경우: 스크린 투영 사용
    float RightDot = FVector::Dot(WorldAxis, CameraRight);
    float UpDot = FVector::Dot(WorldAxis, CameraUp);
    
    // DirectX 스크린 좌표계 고려 (Y축 반전)
    FVector2D ScreenDirection = FVector2D(RightDot, -UpDot);
    
    // 안전한 정규화 (최소 길이 보장)
    float Length = ScreenDirection.Length();
    if (Length > 0.001f)
    {
        return ScreenDirection * (1.0f / Length);
    }
    
    // 예외 상황: 기본 X축 방향
    return FVector2D(1.0f, 0.0f);
}

void AGizmoActor::OnDrag(AActor* Target, uint32 GizmoAxis, float MouseDeltaX, float MouseDeltaY, const ACameraActor* Camera)
{
    if (!Target || !Camera || GizmoAxis == 0)
        return;

    // 화면 크기 정보 가져오기
    FVector2D ScreenSize = UInputManager::GetInstance().GetScreenSize();
    float Sensitivity = 0.05f; // 이동 민감도 조절

    // 마우스 델타를 정규화 (-1 ~ 1)
    FVector2D MouseDelta = FVector2D(MouseDeltaX / ScreenSize.X, MouseDeltaY / ScreenSize.Y);

    FVector Axis{};
    FVector GizmoPosition = GetActorLocation();

    // ────────────── World / Local 축 선택 ──────────────
    if (CurrentSpace == EGizmoSpace::World)
    {
        switch (GizmoAxis)
        {
        case 1: Axis = FVector(1, 0, 0); break;
        case 2: Axis = FVector(0, 1, 0); break;
        case 3: Axis = FVector(0, 0, 1); break;
        }
    }
    else if (CurrentSpace == EGizmoSpace::Local)
    {
        switch (GizmoAxis)
        {
        case 1: Axis = Target->GetActorRight();   break; // Local X
        case 2: Axis = Target->GetActorForward(); break; // Local Y
        case 3: Axis = Target->GetActorUp();      break; // Local Z
        }
    }

    // ────────────── 모드별 처리 ──────────────
    switch (CurrentMode)
    {
    case EGizmoMode::Translate:
    {
        // 안정적인 축 방향 계산
        FVector2D ScreenAxis = GetStableAxisDirection(Axis, Camera);

        // 스크린 공간에서 마우스 이동과 축 방향의 내적으로 이동량 계산
        float Movement = (MouseDelta.X * ScreenAxis.X + MouseDelta.Y * ScreenAxis.Y) * Sensitivity * 200.0f;

        // 일관된 방향으로 이동 (Y축 특수 처리 제거)
        FVector CurrentLocation = Target->GetActorLocation();
        Target->SetActorLocation(CurrentLocation + Axis * Movement);

        break;
    }
    case EGizmoMode::Scale:
    {
        // Determine axis for screen projection
        if (CurrentSpace == EGizmoSpace::World)
        {
            switch (GizmoAxis)
            {
            case 1: Axis = FVector(1, 0, 0); break;
            case 2: Axis = FVector(0, 1, 0); break;
            case 3: Axis = FVector(0, 0, 1); break;
            }
        }
        else if (CurrentSpace == EGizmoSpace::Local)
        {
            switch (GizmoAxis)
            {
            case 1: Axis = Target->GetActorRight();   break; // Local X
            case 2: Axis = Target->GetActorForward(); break; // Local Y
            case 3: Axis = Target->GetActorUp();      break; // Local Z
            }
        }

        // 안정적인 축 방향 계산
        FVector2D ScreenAxis = GetStableAxisDirection(Axis, Camera);

        // 스크린 공간에서 마우스 이동과 축 방향의 내적으로 스케일 변화량 계산
        float Movement = (MouseDelta.X * ScreenAxis.X + MouseDelta.Y * ScreenAxis.Y) * Sensitivity * 50.0f;

        FVector NewScale = Target->GetActorScale();

        // Apply movement to the correct local axis based on which gizmo was dragged
        switch (GizmoAxis)
        {
            case 1: // X Axis
                NewScale.X += Movement;
                break;
            case 2: // Y Axis
                NewScale.Y += Movement;
                break;
            case 3: // Z Axis
                NewScale.Z += Movement;
                break;
        }

        Target->SetActorScale(NewScale);

        break;
    }
    case EGizmoMode::Rotate:
    {
        float RotationSpeed = 0.005f;
        float DeltaAngleX = MouseDeltaX * RotationSpeed;
        float DeltaAngleY = MouseDeltaY * RotationSpeed;

        float Angle = DeltaAngleX + DeltaAngleY;

        // 로컬 모드일 경우 축을 Target 로컬 축으로
        FVector RotationAxis = Axis.GetSafeNormal();

        // = MakeQuatFromAxisAngle(RotationAxis.X, Angle);
        FQuat DeltaQuat{};
        FQuat CurrentRot = Target->GetActorRotation();
        if (CurrentSpace == EGizmoSpace::World)
        {

            switch (GizmoAxis)
            {
            case 1: // X축 회전
            {
                // 마우스 X → 카메라 Up 축 기반
                FQuat RotByX = MakeQuatFromAxisAngle(FVector(-1, 0, 0), DeltaAngleX);
                // 마우스 Y → 카메라 Right 축 기반
                FQuat RotByY = MakeQuatFromAxisAngle(FVector(-1, 0, 0), DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            case 2: // Y축 회전
            {
                FQuat RotByX = MakeQuatFromAxisAngle(FVector(0, -1, 0), DeltaAngleX);
                FQuat RotByY = MakeQuatFromAxisAngle(FVector(0, -1, 0), DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            case 3: // Z축 회전
            {
                FQuat RotByX = MakeQuatFromAxisAngle(FVector(0, 0, -1), DeltaAngleX);
                FQuat RotByY = MakeQuatFromAxisAngle(FVector(0, 0, -1), DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            }
            FQuat NewRot = DeltaQuat * CurrentRot; // 월드 기준 회전
            Target->SetActorRotation(NewRot);
        }
        else
        {
            float RotationSpeed = 0.005f;
            float DeltaAngleX = MouseDeltaX * RotationSpeed;
            float DeltaAngleY = MouseDeltaY * RotationSpeed;

            float Angle = DeltaAngleX + DeltaAngleY;

            // 로컬 모드일 경우 축을 Target 로컬 축으로
            FVector RotationAxis = -Axis.GetSafeNormal();

            //FQuat DeltaQuat = MakeQuatFromAxisAngle(RotationAxis, Angle);

            FQuat CurrentRot = Target->GetActorRotation();

            switch (GizmoAxis)
            {
            case 1: // X축 회전
            {
                // 마우스 X → 카메라 Up 축 기반
                FQuat RotByX = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleX);
                // 마우스 Y → 카메라 Right 축 기반
                FQuat RotByY = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            case 2: // Y축 회전
            {
                FQuat RotByX = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleX);
                FQuat RotByY = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            case 3: // Z축 회전
            {
                FQuat RotByX = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleX);
                FQuat RotByY = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            }

            FQuat NewRot = DeltaQuat * CurrentRot; // 월드 기준 회전

            Target->SetActorRotation(NewRot);


            break;
        }
    }
    }
}

void AGizmoActor::ProcessGizmoInteraction(float DeltaSeconds)
{
    if (!TargetActor || !CameraActor) return;

    UpdateGizmoPosition();

    ProcessGizmoModeSwitch();

    // 기즈모 드래그
    ProcessGizmoDragging(DeltaSeconds);

    // 기즈모 호버링
    if (!InputManager->GetIsGizmoDragging())
    {
        ProcessGizmoHovering();
    }
}

void AGizmoActor::UpdateGizmoPosition()
{
    if (!TargetActor) return;

    SetActorLocation(TargetActor->GetActorLocation());
}

void AGizmoActor::ProcessGizmoHovering()
{
    if (!CameraActor) return;

    uint32 HoveringResult = CPickingSystem::IsHoveringGizmo(this, CameraActor);

    // 드래그 시작 감지
    if (!InputManager->GetIsGizmoDragging() && HoveringResult > 0 && InputManager->IsMouseButtonPressed(LeftButton))
    {
        InputManager->SetIsGizmoDragging(true);
        InputManager->SetDraggingAxis(HoveringResult);
        UE_LOG("기즈모 드래그 시작: 축 %d", HoveringResult);
    }
}

void AGizmoActor::ProcessGizmoDragging(float DeltaSeconds)
{
    if (!InputManager->GetIsGizmoDragging() || !TargetActor || !CameraActor) return;

    if (InputManager->IsMouseButtonDown(LeftButton))
    {
        FVector2D MouseDelta = InputManager->GetMouseDelta();
        if ((MouseDelta.X * MouseDelta.X + MouseDelta.Y * MouseDelta.Y) > 0.0f)
        {
            OnDrag(TargetActor, InputManager->GetDraggingAxis(), MouseDelta.X, MouseDelta.Y, CameraActor);

            SetActorLocation(TargetActor->GetActorLocation());
        }
    }

    if (InputManager->IsMouseButtonReleased(LeftButton))
    {
        InputManager->SetIsGizmoDragging(false);
        InputManager->SetDraggingAxis(0);
        SetSpaceWorldMatrix(CurrentSpace, TargetActor);
    }
}

void AGizmoActor::ProcessGizmoModeSwitch()
{
    // 스페이스 키로 기즈모 모드 전환
    if (InputManager->IsKeyPressed(VK_SPACE))
    {
        int GizmoModeIndex = static_cast<int>(GetMode());
        GizmoModeIndex = (GizmoModeIndex + 1) % 3;  // 3 = enum 개수
        EGizmoMode NewGizmoMode = static_cast<EGizmoMode>(GizmoModeIndex);
        NextMode(NewGizmoMode);
    }
}

void AGizmoActor::UpdateComponentVisibility()
{
    // 선택된 액터가 없으면 모든 기즈모 컴포넌트를 비활성화
    bool bHasSelection = SelectionManager && SelectionManager->HasSelection();
    
    // Arrow Components (Translate 모드)
    bool bShowArrows = bHasSelection && (CurrentMode == EGizmoMode::Translate);
    if (ArrowX) ArrowX->SetActive(bShowArrows);
    if (ArrowY) ArrowY->SetActive(bShowArrows);
    if (ArrowZ) ArrowZ->SetActive(bShowArrows);
    
    // Rotate Components (Rotate 모드)
    bool bShowRotates = bHasSelection && (CurrentMode == EGizmoMode::Rotate);
    if (RotateX) RotateX->SetActive(bShowRotates);
    if (RotateY) RotateY->SetActive(bShowRotates);
    if (RotateZ) RotateZ->SetActive(bShowRotates);
    
    // Scale Components (Scale 모드)
    bool bShowScales = bHasSelection && (CurrentMode == EGizmoMode::Scale);
    if (ScaleX) ScaleX->SetActive(bShowScales);
    if (ScaleY) ScaleY->SetActive(bShowScales);
    if (ScaleZ) ScaleZ->SetActive(bShowScales);
}
