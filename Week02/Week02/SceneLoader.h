#pragma once

#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"   
#include "Vector.h"
#include "UEContainer.h"
using namespace json;

struct FPrimitiveData
{
    FVector Location;
    FVector Rotation;
    FVector Scale;
    FString Type;
};

class FSceneLoader
{
public:
    static TArray<FPrimitiveData> Load(const FString& FileName);

public:
    static void Save(TArray<FPrimitiveData> InPrimitiveData, const FString& SceneName);

private:
    static TArray<FPrimitiveData> Parse(const JSON& Json);
};