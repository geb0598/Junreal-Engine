#pragma once
#include "Actor.h"
#include "Enums.h"

class UGizmoArrowComponent;
class UGizmoScaleComponent;
class UGizmoRotateComponent;
class ACameraActor;
class USelectionManager;
class UInputManager;
class UUIManager;
class CPickingSystem;

class AGizmoActor : public AActor
{
public:
    DECLARE_CLASS(AGizmoActor, AActor)
    AGizmoActor();

    virtual void Tick(float DeltaSeconds) override;
protected:
    ~AGizmoActor() override;

public:

// ────────────────
// Getter Functions
// ────────────────
    UGizmoArrowComponent* GetArrowX() const { return ArrowX; }
    UGizmoArrowComponent* GetArrowY() const { return ArrowY; }
    UGizmoArrowComponent* GetArrowZ() const { return ArrowZ; }
    UGizmoScaleComponent* GetScaleX() const { return ScaleX; }
    UGizmoScaleComponent* GetScaleY() const { return ScaleY; }
    UGizmoScaleComponent* GetScaleZ() const { return ScaleZ; }
    UGizmoRotateComponent* GetRotateX() const { return RotateX; }
    UGizmoRotateComponent* GetRotateY() const { return RotateY; }
    UGizmoRotateComponent* GetRotateZ() const { return RotateZ; }
    void SetMode(EGizmoMode NewMode);
    EGizmoMode GetMode();
    void SetSpaceWorldMatrix(EGizmoSpace NewSpace, AActor* PickedActor);
    void SetSpace(EGizmoSpace NewSpace) { CurrentSpace = NewSpace; }
    EGizmoSpace GetSpace() const { return CurrentSpace; }

    bool GetbRender() const { return bRender; }
    void SetbRender(bool bInRender) { bRender = bInRender; }

    bool GetbIsHovering() const { return bIsHovering; }
    void SetbIsHovering(bool bInIsHovering) { bIsHovering = bInIsHovering; }


    void NextMode(EGizmoMode GizmoMode);
    TArray<USceneComponent*>* GetGizmoComponents();

    
    EGizmoMode GetGizmoMode() const;

    void OnDrag(AActor* Target, uint32 GizmoAxis, float MouseDeltaX, float MouseDeltaY, const ACameraActor* Camera);
    
    // Gizmo interaction methods
    void SetTargetActor(AActor* InTargetActor) { TargetActor = InTargetActor; Tick(0.f);  }
    AActor* GetTargetActor() const { return TargetActor; }
    void SetCameraActor(ACameraActor* InCameraActor) { CameraActor = InCameraActor; }
    ACameraActor* GetCameraActor() const { return CameraActor; }

protected:

    UGizmoArrowComponent* ArrowX;
    UGizmoArrowComponent* ArrowY;
    UGizmoArrowComponent* ArrowZ;
    TArray<USceneComponent*> GizmoArrowComponents;

    UGizmoScaleComponent* ScaleX;
    UGizmoScaleComponent* ScaleY;
    UGizmoScaleComponent* ScaleZ;
    TArray<USceneComponent*> GizmoScaleComponents;

    UGizmoRotateComponent* RotateX;
    UGizmoRotateComponent* RotateY;
    UGizmoRotateComponent* RotateZ;
    TArray<USceneComponent*> GizmoRotateComponents;
    bool bRender = false;
    bool bIsHovering = false;
    EGizmoMode CurrentMode;
    EGizmoSpace CurrentSpace = EGizmoSpace::World;
    
    // Interaction state
    AActor* TargetActor = nullptr;
    ACameraActor* CameraActor = nullptr;
    
    // Manager references
    USelectionManager* SelectionManager = nullptr;
    UInputManager* InputManager = nullptr;
    UUIManager* UIManager = nullptr;
    
    // Gizmo interaction methods
    void ProcessGizmoInteraction(float DeltaSeconds);
    void ProcessGizmoHovering();
    void ProcessGizmoDragging(float DeltaSeconds);
    void ProcessGizmoModeSwitch();
    void UpdateGizmoPosition();
    void UpdateComponentVisibility();
};




