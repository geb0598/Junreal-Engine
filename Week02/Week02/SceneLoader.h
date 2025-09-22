#pragma once

#include <fstream>
#include <sstream>

#include "nlohmann/json.hpp"   
#include "Vector.h"
#include "UEContainer.h"
using namespace json;

struct FPrimitiveData
{
    uint32 UUID = 0;
    FVector Location;
    FVector Rotation;
    FVector Scale;
    FString Type;
    FString ObjStaticMeshAsset;
};

class FSceneLoader
{
public:
    static TArray<FPrimitiveData> Load(const FString& FileName);

public:
    static void Save(TArray<FPrimitiveData> InPrimitiveData, const FString& SceneName);

    // 씬 파일에서 NextUUID 메타만 읽어오는 헬퍼
    static bool TryReadNextUUID(const FString& FilePath, uint32& OutNextUUID);

private:
    static TArray<FPrimitiveData> Parse(const JSON& Json);
};