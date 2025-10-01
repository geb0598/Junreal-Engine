#include "pch.h"

FString UObject::GetName()
{
    return ObjectName.ToString();
}

FString UObject::GetComparisonName()
{
    return FString();
}

UObject* UObject::GetOuter() const
{
	return nullptr;
}

UWorld* UObject::GetWorld() const
{
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}

	return nullptr;
}

UObject* UObject::Duplicate()
{
	UObject* NewObject = new UObject(*this);
	NewObject->DuplicateSubObjects();

	return NewObject;
}
