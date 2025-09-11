#include "GizmoActor.h"
#include "GizmoArrowComponent.h"
#include "GizmoScaleComponent.h"
#include "GizmoRotateComponent.h"
#include "CameraActor.h"
#include"InputManager.h"
#include <cmath>

AGizmoActor::AGizmoActor()
{
    Name = "Gizmo Actor";

    //======= Arrow Component 생성 =======
    ArrowX = NewObject<UGizmoArrowComponent>();
    ArrowY = NewObject<UGizmoArrowComponent>();
    ArrowZ = NewObject<UGizmoArrowComponent>();

    ArrowX->SetDirection(FVector(1.0f, 0.0f, 0.0f));
    ArrowY->SetDirection(FVector(0.0f, 1.0f, 0.0f));
    ArrowZ->SetDirection(FVector(0.0f, 0.0f, 1.0f));

    ArrowX->SetColor(FVector(1.0f, 0.0f, 0.0f));
    ArrowY->SetColor(FVector(0.0f, 1.0f, 0.0f));
    ArrowZ->SetColor(FVector(0.0f, 0.0f, 1.0f));

    ArrowX->SetupAttachment(RootComponent);
    ArrowY->SetupAttachment(RootComponent);
    ArrowZ->SetupAttachment(RootComponent);

    ArrowX->SetRelativeScale({1, 3, 1});
    ArrowY->SetRelativeScale({1, 3, 1});
    ArrowZ->SetRelativeScale({1, 3, 1});

    AddComponent(ArrowX);
    AddComponent(ArrowY);
    AddComponent(ArrowZ);
    GizmoArrowComponents.Add(ArrowX);
    GizmoArrowComponents.Add(ArrowY);   
    GizmoArrowComponents.Add(ArrowZ);
    ArrowX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(-90, 0, 0))); // X축
    ArrowZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90))); // Z축

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
    RotateX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // X축
    RotateY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90))); // Z축

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

    AddComponent(ScaleX);
    AddComponent(ScaleY);
    AddComponent(ScaleZ);
    GizmoScaleComponents.Add(ScaleX);
    GizmoScaleComponents.Add(ScaleY);
    GizmoScaleComponents.Add(ScaleZ);
    ScaleX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // X축
    ScaleY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90))); // Z축

    CurrentMode = EGizmoMode::Translate;
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
        if (ArrowX)  ArrowX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, -90))); // X축//빨
        if (ArrowY) ArrowY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(90, 0, 0)));//초
        if (ArrowZ)  ArrowZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 0))); // Z축//파


        if (ScaleX) ScaleX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // X축//빨
        if (ScaleY) ScaleY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 00))); // Z축//초
        if (ScaleZ) ScaleZ->SetRelativeRotation(FQuat::MakeFromEuler(FVector(-90, 0, 0)));//파


        if (RotateX)         RotateX->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 90, 0))); // X축
        if (RotateY)         RotateY->SetRelativeRotation(FQuat::MakeFromEuler(FVector(0, 0, 90))); // Z축

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
    if (ArrowX)  ArrowX->SetRelativeRotation(TargetRot *FQuat::MakeFromEuler(FVector(0, 0, -90))  ); // X축//빨
    if (ArrowY) ArrowY->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(90, 0, 0)) );//초
    if (ArrowZ)  ArrowZ->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, 0)) ); // Z축//파


    if (ScaleX) ScaleX->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 90, 0)) ); // X축//빨
    if (ScaleY) ScaleY->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, 00)) ); // Z축//초
    if (ScaleZ) ScaleZ->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(-90, 0, 0)) );//파


    if (RotateX)         RotateX->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 90, 0)) ); // X축
    if (RotateY)         RotateY->SetRelativeRotation(TargetRot * FQuat::MakeFromEuler(FVector(0, 0, 90)) ); // Z축

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
        case 3: Axis = FVector(0, 1, 0); break;
        case 2: Axis = FVector(0, 0, 1); break;
        }
    }
    else if (CurrentSpace == EGizmoSpace::Local)
    {
        switch (GizmoAxis)
        {
        case 1: Axis = Target->GetActorRight();   break; // Local X
        case 3: Axis = Target->GetActorForward(); break; // Local Y
        case 2: Axis = Target->GetActorUp();      break; // Local Z
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
        // Scale 모드에서는 월드 축 고정 사용
        switch (GizmoAxis)
        {
        case 1: Axis = FVector(1, 0, 0); break;
        case 3: Axis = FVector(0, 1, 0); break;
        case 2: Axis = FVector(0, 0, 1); break;
        }
        
        // 안정적인 축 방향 계산
        FVector2D ScreenAxis = GetStableAxisDirection(Axis, Camera);
        
        // 스크린 공간에서 마우스 이동과 축 방향의 내적으로 스케일 변화량 계산
        float Movement = (MouseDelta.X * ScreenAxis.X + MouseDelta.Y * ScreenAxis.Y) * Sensitivity * 50.0f;
        
        // 일관된 방향으로 스케일 조정 (Y축 특수 처리 제거)
        FVector CurrentScale = Target->GetActorScale();
        Target->SetActorScale(CurrentScale + Axis * Movement);
       
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
            case 3: // Y축 회전
            {
                FQuat RotByX = MakeQuatFromAxisAngle(FVector(0, -1, 0), DeltaAngleX);
                FQuat RotByY = MakeQuatFromAxisAngle(FVector(0, -1, 0), DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            case 2: // Z축 회전
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
            FVector RotationAxis = Axis.GetSafeNormal();

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
            case 3: // Y축 회전
            {
                FQuat RotByX = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleX);
                FQuat RotByY = MakeQuatFromAxisAngle(RotationAxis, DeltaAngleY);
                DeltaQuat = RotByX * RotByY;
                break;
            }
            case 2: // Z축 회전
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


