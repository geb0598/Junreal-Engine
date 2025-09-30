#include "pch.h"
#include "Level.h"

ULevel::ULevel()
{
}

ULevel::~ULevel()
{
	for (int i = Actors.Num() - 1; i >= 0; --i)
	{
		if (Actors[i] != nullptr)
		{
			//delete Actors[i]; 
			Actors[i] = nullptr;
		}
	}
	Actors.Empty();
}

void ULevel::AddActor(AActor* InActor)
{
	if (InActor)	
	{
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

const TArray<AActor*>& ULevel::GetActors() const
{
	return Actors;
}
