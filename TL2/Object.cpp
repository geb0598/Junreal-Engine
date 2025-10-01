#include "pch.h"

FString UObject::GetName()
{
    return ObjectName.ToString();
}

FString UObject::GetComparisonName()
{
    return FString();
}

void UObject::DuplicateSubObjects()
{
    // Base class has no sub objects to deep copy
    // This is intended to be ovveridden by derived class   
}

// After shallow copy, call DuplicateSubObjects (virtual function)
UObject* UObject::Duplicate()
{
    // shallow copy
    UObject* NewObject = new UObject(*this); 

    // deep copy if object has sub objects
    NewObject->DuplicateSubObjects();

    return NewObject;
}


