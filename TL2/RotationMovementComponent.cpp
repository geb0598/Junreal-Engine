#include "pch.h"
#include "RotationMovementComponent.h"

URotationMovementComponent::URotationMovementComponent()
	: RotationRate(FVector(0.f, 80.f, 0.f))
	, PivotTranslation(FVector(0.f, 0.f, 0.f))
	, bRotationInLocalSpace(true)
{
	bCanEverTick = true;
	WorldTickMode = EComponentWorldTickMode::PIEOnly; // PIE에서만 작동
}

void URotationMovementComponent::TickComponent(float DeltaSeconds)
{
	UActorComponent::TickComponent(DeltaSeconds);
	if (!UpdatedComponent)
	{
		return;
	}

	// Rotations Per Second
	const FVector DeltaRotation = RotationRate * DeltaSeconds;
	const FQuat DeltaQuat = FQuat::MakeFromEuler(DeltaRotation);

	// Apply Rotation
	if (PivotTranslation.SizeSquared() < KINDA_SMALL_NUMBER)
	{
		if (bRotationInLocalSpace)
		{
			UpdatedComponent->AddLocalRotation(DeltaQuat);
		}
		else
		{
			UpdatedComponent->AddWorldRotation(DeltaQuat);
		}
		return;
	}

	// --- [PivotTranslation이 있을 경우] ---
	// 현재 월드 트랜스폼 가져오기
	FTransform WorldTransform = UpdatedComponent->GetWorldTransform();

	// 로컬 Pivot을 월드 공간으로 변환
	FVector WorldPivot = WorldTransform.TransformPosition(PivotTranslation);

	// 회전 적용 후 위치 재계산
	FQuat CurrentRot = WorldTransform.Rotation;
	FQuat NewRot = (bRotationInLocalSpace)
		? (CurrentRot * DeltaQuat)	// 로컬 축 회전
		: (DeltaQuat * CurrentRot); // 월드 축 회전

	// 현재 위치를 피벗 기준으로 회전
	FVector WorldLoc = WorldTransform.Translation;
	FVector RotatedLoc = WorldPivot + NewRot.RotateVector(WorldLoc - WorldPivot);

	// 적용
	UpdatedComponent->SetWorldLocationAndRotation(RotatedLoc, NewRot);
}

UObject* URotationMovementComponent::Duplicate()
{
	URotationMovementComponent* NewComp = Cast<URotationMovementComponent>(UMovementComponent::Duplicate());
	if (NewComp)
	{
		NewComp->RotationRate = RotationRate;
		NewComp->PivotTranslation = PivotTranslation;
		NewComp->bRotationInLocalSpace = bRotationInLocalSpace;
	}
	return NewComp;
}

void URotationMovementComponent::DuplicateSubObjects()
{
	UMovementComponent::DuplicateSubObjects();
}