#include "pch.h"
#include "SceneLoader.h"

#include <algorithm>


TArray<FPrimitiveData> FSceneLoader::Load(const FString& FileName)
{
    std::ifstream file(FileName);
    if (!file.is_open())
    {
        std::cerr << "파일을 열 수 없습니다: " << FileName << std::endl;
        return {};
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    try {
        JSON j = JSON::Load(content);
        return Parse(j);
    }
    catch (const std::exception& e) {
        std::cerr << "JSON 파싱 실패: " << e.what() << std::endl;
        return {};
    }
}

void FSceneLoader::Save(TArray<FPrimitiveData> InPrimitiveData, const FString& SceneName)
{
    // using namespace json;

    JSON Root = JSON::Make(JSON::Class::Object);
    Root["Version"] = 1;

    uint32 NextUUID = 0;
    for (UObject* Object: GUObjectArray)
    {
        NextUUID = std::max(Object->UUID, NextUUID);
    }
    Root["NextUUID"] = (long)NextUUID;

    JSON Prims = JSON::Make(JSON::Class::Object);

    for (size_t i = 0; i < InPrimitiveData.size(); ++i)
    {
        const FPrimitiveData& Data = InPrimitiveData[i];

        JSON Obj = JSON::Make(JSON::Class::Object);
        Obj["Location"] = Array(Data.Location.X, Data.Location.Y, Data.Location.Z);
        Obj["Rotation"] = Array(Data.Rotation.X, Data.Rotation.Y, Data.Rotation.Z);
        Obj["Scale"] = Array(Data.Scale.X, Data.Scale.Y, Data.Scale.Z);
        Obj["Type"] = Data.Type;

        Prims[std::to_string(i)] = Obj;
    }

    Root["Primitives"] = Prims;

    // Build filename: ensure ".scene" extension
    std::string FileName = SceneName;
    const std::string Ext = ".scene";
    if (FileName.size() < Ext.size() || FileName.substr(FileName.size() - Ext.size()) != Ext)
    {
        FileName += Ext;
    }

    // Create or truncate the file explicitly
    std::ofstream OutFile(FileName.c_str(), std::ios::out | std::ios::trunc);
    if (OutFile.is_open())
    {
        OutFile << Root.dump(1, "  "); // 들여쓰기 2칸
        OutFile.close();
    }
}

TArray<FPrimitiveData> FSceneLoader::Parse(const JSON& Json)
{
    TArray<FPrimitiveData> primitives;

    if (!Json.hasKey("Primitives"))
    {
        std::cerr << "Primitives 섹션이 존재하지 않습니다." << std::endl;
        return primitives;
    }

    auto primitivesJson = Json.at("Primitives");
    for (auto& kv : primitivesJson.ObjectRange())
    {
        const JSON& value = kv.second;

        FPrimitiveData data;

        auto loc = value.at("Location");
        data.Location = FVector(
            (float)loc[0].ToFloat(),   
            (float)loc[1].ToFloat(),   
            (float)loc[2].ToFloat()    
        );

        auto rot = value.at("Rotation");
        data.Rotation = FVector(
            (float)rot[0].ToFloat(),   
            (float)rot[1].ToFloat(),   
            (float)rot[2].ToFloat()    
        );

        auto scale = value.at("Scale");
        data.Scale = FVector(
            (float)scale[0].ToFloat(),
            (float)scale[1].ToFloat(), 
            (float)scale[2].ToFloat()  
        );

        data.Type = value.at("Type").ToString();

        primitives.push_back(data);
    }

    return primitives;
}
