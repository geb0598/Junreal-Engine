#pragma once
#include "Actor.h"
class UCameraComponent;
class UUIManager;
class UInputManager;
//enum class ECameraProjectionMode
//{
//    Perspective,
//    Orthographic
//};
class ACameraActor : public AActor
{
public:
    DECLARE_CLASS(ACameraActor, AActor)
    ACameraActor();
    virtual void Tick(float DeltaSeconds) override;

protected:
    ~ACameraActor() override;

public:
    UCameraComponent* GetCameraComponent() const { return CameraComponent; }

   // ECameraProjectionMode ProjectionMode = ECameraProjectionMode::Perspective;

    // Matrices
    FMatrix GetViewMatrix() const;
    FMatrix GetProjectionMatrix() const;
    FMatrix GetViewProjectionMatrix() const;

    // Directions (world)
    FVector GetForward() const;
    FVector GetRight() const;
    FVector GetUp() const;

    // Camera control methods
    void SetMouseSensitivity(float Sensitivity) { MouseSensitivity = Sensitivity; }
    void SetMoveSpeed(float Speed) { CameraMoveSpeed = Speed; }
    
    // Camera state getters
    float GetCameraYaw() const { return CameraYawDeg; }
    float GetCameraPitch() const { return CameraPitchDeg; }
    void SetCameraYaw(float Yaw) { CameraYawDeg = Yaw; }
    void SetCameraPitch(float Pitch) { CameraPitchDeg = Pitch; }

    float GetCameraSpeed() { return CameraMoveSpeed; }
    void SetCameraSpeed(float InSpeed) { CameraMoveSpeed = InSpeed; EditorINI["CameraSpeed"] = std::to_string(CameraMoveSpeed); }

private:
    UCameraComponent* CameraComponent = nullptr;
    
    // Camera control parameters
    float MouseSensitivity = 0.1f;  // 기존 World에서 사용하던 값으로 조정
    float CameraMoveSpeed = 10.0f;
    
    // Camera rotation state (cumulative)
    float CameraYawDeg = 0.0f;   // World Up(Y) based Yaw (unlimited accumulation)
    float CameraPitchDeg = 0.0f; // Local Right based Pitch (limited)
    
    // Camera input processing methods
    void ProcessEditorCameraInput(float DeltaSeconds);
    void ProcessCameraRotation(float DeltaSeconds);
    void ProcessCameraMovement(float DeltaSeconds);

};

