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

void FSceneLoader::Save(TArray<FPrimitiveData> InPrimitiveData, const FPerspectiveCameraData* InCameraData, const FString& SceneName)
{
    // 상단 메타 정보
    uint32 NextUUID = UObject::PeekNextUUID();

    // 파일 경로 구성: SceneName이 디렉터리를 포함하지 않으면 "Scene/"를 붙임
    namespace fs = std::filesystem;
    fs::path outPath(SceneName);

    if (!outPath.has_parent_path())
    {
        outPath = fs::path("Scene") / outPath; // Scene/Name
    }

    // 확장자 보정(.Scene)
    if (outPath.extension().string() != ".Scene")
    {
        outPath.replace_extension(".Scene");
    }

    // 상위 디렉터리 생성 보장
    std::error_code ec;
    fs::create_directories(outPath.parent_path(), ec);

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

    // Primitives
    oss << "  \"Primitives\": {\n";
    for (size_t i = 0; i < InPrimitiveData.size(); ++i)
    {
        const FPrimitiveData& Data = InPrimitiveData[i];
        oss << "    \"" << Data.UUID << "\": {\n";
        oss << "      \"ObjStaticMeshAsset\": " << "\"" << Data.ObjStaticMeshAsset << "\",\n";
        writeVec3("Location", Data.Location, 6); oss << ",\n";
        writeVec3("Rotation", Data.Rotation, 6); oss << ",\n";
        writeVec3("Scale", Data.Scale, 6); oss << ",\n";
        oss << "      \"Type\": " << "\"" << Data.Type << "\"\n";
        oss << "    }" << (i + 1 < InPrimitiveData.size() ? "," : "") << "\n";
    }
    oss << "  }";

    // PerspectiveCamera
    if (InCameraData)
    {
        oss << ",\n";
        oss << "  \"PerspectiveCamera\": {\n";
        writeVec3("Location", InCameraData->Location, 4); oss << ",\n";
        writeVec3("Rotation", InCameraData->Rotation, 4); oss << ",\n";
        oss << "    \"FOV\": " << InCameraData->FOV << ",\n";
        oss << "    \"NearClip\": " << InCameraData->NearClip << ",\n";
        oss << "    \"FarClip\": " << InCameraData->FarClip << "\n";
        oss << "  }\n";
    }
    else
    {
        oss << "\n";
    }

    oss << "}\n";

    const std::string finalPath = outPath.make_preferred().string();
    std::ofstream OutFile(finalPath.c_str(), std::ios::out | std::ios::trunc);
    if (OutFile.is_open())
    {
        OutFile << oss.str();
        OutFile.close();
    }
    else
    {
        std::cerr << "Scene save failed. Cannot open file: " << finalPath << std::endl;
    }
}

// ─────────────────────────────────────────────
// NextUUID 메타만 읽어오는 간단한 헬퍼
// 저장 포맷상 "NextUUID"는 "마지막으로 사용된 UUID"이므로,
// 호출 측에서 +1 해서 SetNextUUID 해야 함
// ─────────────────────────────────────────────
bool FSceneLoader::TryReadNextUUID(const FString& FilePath, uint32& OutNextUUID)
{
    std::ifstream file(FilePath);
    if (!file.is_open())
    {
        return false;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    try
    {
        JSON j = JSON::Load(content);
        if (j.hasKey("NextUUID"))
        {
            // 정수 파서가 없으면 ToFloat로 받아서 캐스팅
			OutNextUUID = static_cast<uint32>(j.at("NextUUID").ToInt());
            return true;
        }
    }
    catch (...)
    {
        // 무시하고 false 반환
    }
    return false;
}

TArray<FPrimitiveData> FSceneLoader::Parse(const JSON& Json)
{
    TArray<FPrimitiveData> Primitives;

    if (!Json.hasKey("Primitives"))
    {
        std::cerr << "Primitives 섹션이 존재하지 않습니다." << std::endl;
        return Primitives;
    }

    auto PrimitivesJson = Json.at("Primitives");
    for (auto& kv : PrimitivesJson.ObjectRange())
    {
        // kv.first: 키(문자열), kv.second: 값(JSON 객체)
        const std::string& key = kv.first;
        const JSON& value = kv.second;

        FPrimitiveData data;

        // 키를 UUID로 파싱 (숫자가 아니면 0 유지)
        try
        {
            // 공백 제거 후 파싱
            std::string trimmed = key;
            trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), ::isspace), trimmed.end());
            data.UUID = static_cast<uint32>(std::stoul(trimmed));
        }
        catch (...)
        {
            data.UUID = 0; // 레거시 호환: 숫자 키가 아니면 0
        }

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

        if (value.hasKey("ObjStaticMeshAsset"))
        {
            data.ObjStaticMeshAsset = value.at("ObjStaticMeshAsset").ToString();
        }
        else
        {
            data.ObjStaticMeshAsset = "";
        }

        data.Type = value.at("Type").ToString();

        Primitives.push_back(data);
    }

    return Primitives;
}
