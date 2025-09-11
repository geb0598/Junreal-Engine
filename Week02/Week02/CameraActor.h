#pragma once
#include "Actor.h"
class UCameraComponent;
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


private:
    UCameraComponent* CameraComponent = nullptr;

};

