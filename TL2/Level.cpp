﻿#include "pch.h"
#include "Level.h"
#include "DecalComponent.h"
#include "ExponentialHeightFogComponent.h"
#include "PrimitiveComponent.h"
#include "BillboardComponent.h"
#include "FireballComponent.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
	Actors.clear();
}

void ULevel::AddActor(AActor* InActor)
{
	if (InActor)	
	{
		if (InActor->GetClass()->Name == AActor::StaticClass()->Name)
		{
			InActor->InitEmptyActor();

		}
		Actors.Add(InActor);
	}
}

void ULevel::RemoveActor(AActor* InActor)
{
	if (InActor)
	{
		Actors.Remove(InActor);
		//delete InActor;
	}
}

void ULevel::CollectComponentsToRender()
{
	DecalComponentList.clear();
	PrimitiveComponentList.clear();
	BillboardComponentList.clear();
	FogComponentList.clear();
	FireBallComponentList.clear();

	for (AActor* Actor : Actors)
	{
		if (!Actor || Actor->GetActorHiddenInGame())
		{
			continue;
		}

		for (UActorComponent* ActorComponent : Actor->GetComponents())
		{
			if (UDecalComponent * DecalComponent = Cast<UDecalComponent>(ActorComponent))
			{
				DecalComponentList.Add(DecalComponent);
			}
			else if (UBillboardComponent* BillboardComponent = Cast<UBillboardComponent>(ActorComponent))
			{
				BillboardComponentList.Add(BillboardComponent);
			}
			else if (UExponentialHeightFogComponent* FogComponent = Cast<UExponentialHeightFogComponent>(ActorComponent))
			{
				FogComponentList.Add(FogComponent);
			}
			else if (UFireBallComponent* FireBallComponent = Cast<UFireBallComponent>(ActorComponent))
			{
				FireBallComponentList.Add(FireBallComponent);
			}
			else if (UPrimitiveComponent* PrimitiveComponent = Cast<UPrimitiveComponent>(ActorComponent))
			{
				PrimitiveComponentList.Add(PrimitiveComponent);
			}
		}
	}

	DecalComponentList.Sort([](const UDecalComponent* A, const UDecalComponent* B)
		{
			return A->GetSortOrder() < B->GetSortOrder();
		});
}

const TArray<AActor*>& ULevel::GetActors() const
{
	return Actors;
}

TArray<AActor*>& ULevel::GetActors() 
{
	return Actors;
}

template<>
TArray<UExponentialHeightFogComponent*>& ULevel::GetComponentList<UExponentialHeightFogComponent>()
{
	return FogComponentList;
}
template<>
TArray<UBillboardComponent*>& ULevel::GetComponentList<UBillboardComponent>()
{
	return BillboardComponentList;
}
template<>
TArray<UDecalComponent*>& ULevel::GetComponentList<UDecalComponent>()
{
	return DecalComponentList;
}
template<>
TArray<UPrimitiveComponent*>& ULevel::GetComponentList<UPrimitiveComponent>()
{
	return PrimitiveComponentList;
}
template<>
TArray<UFireBallComponent*>& ULevel::GetComponentList<UFireBallComponent>()
{
	return FireBallComponentList;
}