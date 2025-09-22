#include "pch.h"
#include "SceneLoader.h"

#include <algorithm>
#include <iomanip>

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
    // 상단 메타 정보
    uint32 NextUUID = 0;
    for (UObject* Object : GUObjectArray)
    {
        NextUUID = std::max(Object->UUID, NextUUID);
    }

    // 파일명 보정(.Scene 보장)
    std::string FileName = SceneName;
    const std::string Ext = ".Scene";
    if (FileName.size() < Ext.size() || FileName.substr(FileName.size() - Ext.size()) != Ext)
    {
        FileName += Ext;
    }

    // 수동 직렬화로 키 순서를 고정
    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss << std::setprecision(6);

    auto writeVec3 = [&](const char* name, const FVector& v, int indent)
        {
            std::string tabs(indent, ' ');
            oss << tabs << "\"" << name << "\": [ "
                << v.X << ", " << v.Y << ", " << v.Z << " ]";
        };

    oss << "{\n";
    oss << "  \"Version\": 1,\n";
    oss << "  \"NextUUID\": " << NextUUID << ",\n";
    oss << "  \"Primitives\": {\n";

    for (size_t i = 0; i < InPrimitiveData.size(); ++i)
    {
        const FPrimitiveData& Data = InPrimitiveData[i];
        oss << "    \"" << i << "\": {\n";

        // 원하는 순서대로 출력
        oss << "      \"ObjStaticMeshAsset\": " << "\"" << Data.ObjStaticMeshAsset << "\",\n";
        writeVec3("Location", Data.Location, 6); oss << ",\n";
        writeVec3("Rotation", Data.Rotation, 6); oss << ",\n";
        writeVec3("Scale", Data.Scale, 6); oss << ",\n";
        oss << "      \"Type\": " << "\"" << Data.Type << "\"\n";

        oss << "    }" << (i + 1 < InPrimitiveData.size() ? "," : "") << "\n";
    }

    oss << "  }\n";
    oss << "}\n";

    std::ofstream OutFile(FileName.c_str(), std::ios::out | std::ios::trunc);
    if (OutFile.is_open())
    {
        OutFile << oss.str();
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

        // 구버전/신버전 호환
        if (value.hasKey("ObjStaticMeshAsset"))
        {
            data.ObjStaticMeshAsset = value.at("ObjStaticMeshAsset").ToString();
        }
        else
        {
            data.ObjStaticMeshAsset = ""; // 없으면 빈 문자열
        }

        data.Type = value.at("Type").ToString();

        primitives.push_back(data);
    }

    return primitives;
}
