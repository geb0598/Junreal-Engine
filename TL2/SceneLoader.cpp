﻿#include "pch.h"
#include "SceneLoader.h"

#include <algorithm>
#include <iomanip>

static bool ParsePerspectiveCamera(const JSON& Root, FPerspectiveCameraData& OutCam)
{
    if (!Root.hasKey("PerspectiveCamera"))
        return false;

    const JSON& Cam = Root.at("PerspectiveCamera");

    // 배열형 벡터(Location, Rotation) 파싱 (스칼라 실패 시 무시)
    auto readVec3 = [](JSON arr, FVector& outVec)
        {
            try
            {
                outVec = FVector(
                    (float)arr[0].ToFloat(),
                    (float)arr[1].ToFloat(),
                    (float)arr[2].ToFloat());
            }
            catch (...) {} // 실패 시 기본값 유지
        };

    if (Cam.hasKey("Location"))
        readVec3(Cam.at("Location"), OutCam.Location);
    if (Cam.hasKey("Rotation"))
        readVec3(Cam.at("Rotation"), OutCam.Rotation);

    // 스칼라 또는 [스칼라] 모두 허용
    auto readScalarFlexible = [](const JSON& parent, const char* key, float& outVal)
        {
            if (!parent.hasKey(key)) return;
            const JSON& node = parent.at(key);
            try
            {
                // 배열 형태 시도 (예: "FOV": [60.0])
                outVal = (float)node.at(0).ToFloat();
            }
            catch (...)
            {
                // 스칼라 (예: "FOV": 60.0)
                outVal = (float)node.ToFloat();
            }
        };

    readScalarFlexible(Cam, "FOV", OutCam.FOV);
    readScalarFlexible(Cam, "NearClip", OutCam.NearClip);
    readScalarFlexible(Cam, "FarClip", OutCam.FarClip);

    return true;
}

TArray<FPrimitiveData> FSceneLoader::Load(const FString& FileName, FPerspectiveCameraData* OutCameraData)
{
    std::ifstream file(FileName);
    if (!file.is_open())
    {
		UE_LOG("Scene load failed. Cannot open file: %s", FileName.c_str());
        return {};
    }

    std::stringstream Buffer;
    Buffer << file.rdbuf();
    std::string content = Buffer.str();

    try {
        JSON j = JSON::Load(content);

        // 카메라 먼저 파싱
        if (OutCameraData)
        {
            FPerspectiveCameraData Temp{};
            if (ParsePerspectiveCamera(j, Temp))
            {
                *OutCameraData = Temp;
            }
            else
            {
                // 카메라 블록이 없으면 값을 건드리지 않음
            }
        }

        return Parse(j);
    }
    catch (const std::exception& e) {
		UE_LOG("Scene load failed. JSON parse error: %s", e.what());
        return {};
    }
}

void FSceneLoader::Save(TArray<FPrimitiveData> InPrimitiveData, const FPerspectiveCameraData* InCameraData, const FString& SceneName)
{
    uint32 NextUUID = UObject::PeekNextUUID();

    namespace fs = std::filesystem;
    fs::path outPath(SceneName);
    if (!outPath.has_parent_path())
        outPath = fs::path("Scene") / outPath;
    if (outPath.extension().string() != ".Scene")
        outPath.replace_extension(".Scene");
    std::error_code ec;
    fs::create_directories(outPath.parent_path(), ec);

    auto NormalizePath = [](FString Path) -> FString
        {
            // 절대 경로를 프로젝트 기준 상대 경로로 변환
            fs::path absPath = fs::absolute(Path);
            fs::path currentPath = fs::current_path();

            std::error_code ec;
            fs::path relativePath = fs::relative(absPath, currentPath, ec);

            // 상대 경로 변환 실패 시 원본 경로 사용
            FString result = ec ? Path : relativePath.string();

            // 백슬래시를 슬래시로 변환 (크로스 플랫폼 호환)
            for (auto& ch : result)
            {
                if (ch == '\\') ch = '/';
            }
            return result;
        };

    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss << std::setprecision(6);

    auto writeVec3 = [&](const char* name, const FVector& v, int indent)
        {
            std::string tabs(indent, ' ');
            oss << tabs << "\"" << name << "\" : [" << v.X << ", " << v.Y << ", " << v.Z << "]";
        };

    oss << "{\n";
    oss << "  \"Version\" : 1,\n";
    oss << "  \"NextUUID\" : " << NextUUID;

    bool bHasCamera = (InCameraData != nullptr);

    if (bHasCamera)
    {
        oss << ",\n";
        oss << "  \"PerspectiveCamera\" : {\n";
        // 순서: FOV, FarClip, Location, NearClip, Rotation (FOV/Clip들은 단일 요소 배열)
        oss << "    \"FOV\" : [" << InCameraData->FOV << "],\n";
        oss << "    \"FarClip\" : [" << InCameraData->FarClip << "],\n";
        writeVec3("Location", InCameraData->Location, 4); oss << ",\n";
        oss << "    \"NearClip\" : [" << InCameraData->NearClip << "],\n";
        writeVec3("Rotation", InCameraData->Rotation, 4); oss << "\n";
        oss << "  }";
    }

    // Primitives 블록
    oss << (bHasCamera ? ",\n" : ",\n"); // 카메라 없더라도 컴마 후 줄바꿈
    oss << "  \"Primitives\" : {\n";
    for (size_t i = 0; i < InPrimitiveData.size(); ++i)
    {
        const FPrimitiveData& Data = InPrimitiveData[i];
        oss << "    \"" << Data.UUID << "\" : {\n";
        // 순서: Location, ObjStaticMeshAsset, Rotation, Scale, Type
        writeVec3("Location", Data.Location, 6); oss << ",\n";

        FString AssetPath = NormalizePath(Data.ObjStaticMeshAsset);
        oss << "      \"ObjStaticMeshAsset\" : " << "\"" << AssetPath << "\",\n";

        writeVec3("Rotation", Data.Rotation, 6); oss << ",\n";
        writeVec3("Scale", Data.Scale, 6); oss << ",\n";
        oss << "      \"Type\" : " << "\"" << Data.Type << "\"\n";
        oss << "    }" << (i + 1 < InPrimitiveData.size() ? "," : "") << "\n";
    }
    oss << "  }\n";
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
        UE_LOG("Scene save failed. Cannot open file: %s", finalPath.c_str());
    }
}

// ========================================
// Version 2 API Implementation
// ========================================

void FSceneLoader::SaveV2(const FSceneData& SceneData, const FString& SceneName)
{
    namespace fs = std::filesystem;
    fs::path outPath(SceneName);
    if (!outPath.has_parent_path())
        outPath = fs::path("Scene") / outPath;
    if (outPath.extension().string() != ".Scene")
        outPath.replace_extension(".Scene");
    std::error_code ec;
    fs::create_directories(outPath.parent_path(), ec);

    auto NormalizePath = [](FString Path) -> FString
    {
        // 절대 경로를 프로젝트 기준 상대 경로로 변환
        fs::path absPath = fs::absolute(Path);
        fs::path currentPath = fs::current_path();

        std::error_code ec;
        fs::path relativePath = fs::relative(absPath, currentPath, ec);

        // 상대 경로 변환 실패 시 원본 경로 사용
        FString result = ec ? Path : relativePath.string();

        // 백슬래시를 슬래시로 변환 (크로스 플랫폼 호환)
        for (auto& ch : result)
        {
            if (ch == '\\') ch = '/';
        }
        return result;
    };

    std::ostringstream oss;
    oss.setf(std::ios::fixed);
    oss << std::setprecision(6);

    auto writeVec3 = [&](const char* name, const FVector& v, int indent)
    {
        std::string tabs(indent, ' ');
        oss << tabs << "\"" << name << "\" : [" << v.X << ", " << v.Y << ", " << v.Z << "]";
    };

    // Root
    oss << "{\n";
    oss << "  \"Version\" : " << SceneData.Version << ",\n";
    oss << "  \"NextUUID\" : " << SceneData.NextUUID << ",\n";

    // Camera
    oss << "  \"PerspectiveCamera\" : {\n";
    oss << "    \"FOV\" : [" << SceneData.Camera.FOV << "],\n";
    oss << "    \"FarClip\" : [" << SceneData.Camera.FarClip << "],\n";
    writeVec3("Location", SceneData.Camera.Location, 4); oss << ",\n";
    oss << "    \"NearClip\" : [" << SceneData.Camera.NearClip << "],\n";
    writeVec3("Rotation", SceneData.Camera.Rotation, 4); oss << "\n";
    oss << "  },\n";

    // Actors
    oss << "  \"Actors\" : [\n";
    for (size_t i = 0; i < SceneData.Actors.size(); ++i)
    {
        const FActorData& Actor = SceneData.Actors[i];
        oss << "    {\n";
        oss << "      \"UUID\" : " << Actor.UUID << ",\n";
        oss << "      \"Type\" : \"" << Actor.Type << "\",\n";
        oss << "      \"Name\" : \"" << Actor.Name << "\",\n";
        oss << "      \"RootComponentUUID\" : " << Actor.RootComponentUUID << "\n";
        oss << "    }" << (i + 1 < SceneData.Actors.size() ? "," : "") << "\n";
    }
    oss << "  ],\n";

    // Components
    oss << "  \"Components\" : [\n";
    for (size_t i = 0; i < SceneData.Components.size(); ++i)
    {
        const FComponentData& Comp = SceneData.Components[i];
        oss << "    {\n";
        oss << "      \"UUID\" : " << Comp.UUID << ",\n";
        oss << "      \"OwnerActorUUID\" : " << Comp.OwnerActorUUID << ",\n";
        oss << "      \"ParentComponentUUID\" : " << Comp.ParentComponentUUID << ",\n";
        oss << "      \"Type\" : \"" << Comp.Type << "\",\n";
        writeVec3("RelativeLocation", Comp.RelativeLocation, 6); oss << ",\n";
        writeVec3("RelativeRotation", Comp.RelativeRotation, 6); oss << ",\n";
        writeVec3("RelativeScale", Comp.RelativeScale, 6);

        // Type별 속성
        bool bHasTypeSpecificData = false;

        // StaticMeshComponent
        if (Comp.Type.find("StaticMeshComponent") != std::string::npos && !Comp.StaticMesh.empty())
        {
            if (!bHasTypeSpecificData) { oss << ",\n"; bHasTypeSpecificData = true; }
            FString AssetPath = NormalizePath(Comp.StaticMesh);
            oss << "      \"StaticMesh\" : \"" << AssetPath << "\"";

            if (!Comp.Materials.empty())
            {
                oss << ",\n";
                oss << "      \"Materials\" : [";
                for (size_t m = 0; m < Comp.Materials.size(); ++m)
                {
                    oss << "\"" << Comp.Materials[m] << "\"";
                    if (m + 1 < Comp.Materials.size()) oss << ", ";
                }
                oss << "]";
            }
        }

        // DecalComponent, BillboardComponent, SpotLightComponent
        if ((Comp.Type.find("DecalComponent") != std::string::npos ||
             Comp.Type.find("BillboardComponent") != std::string::npos ||
             Comp.Type.find("SpotLightComponent") != std::string::npos) &&
            !Comp.TexturePath.empty())
        {
            if (!bHasTypeSpecificData) { oss << ",\n"; bHasTypeSpecificData = true; }
            else { oss << ",\n"; }
            FString TexturePath = NormalizePath(Comp.TexturePath);
            oss << "      \"TexturePath\" : \"" << TexturePath << "\"";
        }
        if (Comp.Type.find("FireBallComponent") != std::string::npos)
        {
            if (!bHasTypeSpecificData) { oss << ",\n"; bHasTypeSpecificData = true; }
            else { oss << ",\n"; }

            const FFireBallProperty& FB = Comp.FireBallProperty; // FComponentData 내부에 있다고 가정

            oss << "      \"FireBallData\" : {\n";
            oss << "        \"Intensity\" : " << FB.Intensity << ",\n";
            oss << "        \"Radius\" : " << FB.Radius << ",\n";
            oss << "        \"RadiusFallOff\" : " << FB.RadiusFallOff << ",\n";
            oss << "        \"Color\" : [" << FB.Color.R << ", " << FB.Color.G << ", " << FB.Color.B << ", " << FB.Color.A << "]\n";
            oss << "      }";
        }

        oss << "\n";
        oss << "    }" << (i + 1 < SceneData.Components.size() ? "," : "") << "\n";
    }
    oss << "  ]\n";
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
        UE_LOG("Scene save failed. Cannot open file: %s", finalPath.c_str());
    }
}

FSceneData FSceneLoader::LoadV2(const FString& FileName)
{
    FSceneData Result;

    std::ifstream file(FileName);
    if (!file.is_open())
    {
        UE_LOG("Scene load failed. Cannot open file: %s", FileName.c_str());
        return Result;
    }

    std::stringstream Buffer;
    Buffer << file.rdbuf();
    std::string content = Buffer.str();

    try {
        JSON j = JSON::Load(content);
        Result = ParseV2(j);
    }
    catch (const std::exception& e) {
        UE_LOG("Scene load failed. JSON parse error: %s", e.what());
    }

    return Result;
}

FSceneData FSceneLoader::ParseV2(const JSON& Json)
{
    FSceneData Data;

    // Version
    if (Json.hasKey("Version"))
        Data.Version = static_cast<uint32>(Json.at("Version").ToInt());

    // NextUUID
    if (Json.hasKey("NextUUID"))
        Data.NextUUID = static_cast<uint32>(Json.at("NextUUID").ToInt());

    // Camera
    if (Json.hasKey("PerspectiveCamera"))
    {
        ParsePerspectiveCamera(Json, Data.Camera);
    }

    // Actors
    if (Json.hasKey("Actors"))
    {
        const JSON& ActorsJson = Json.at("Actors");
        for (size_t i = 0; i < ActorsJson.size(); ++i)
        {
            const JSON& ActorJson = ActorsJson.at(i);
            FActorData Actor;

            if (ActorJson.hasKey("UUID"))
                Actor.UUID = static_cast<uint32>(ActorJson.at("UUID").ToInt());
            if (ActorJson.hasKey("Type"))
                Actor.Type = ActorJson.at("Type").ToString();
            if (ActorJson.hasKey("Name"))
                Actor.Name = ActorJson.at("Name").ToString();
            if (ActorJson.hasKey("RootComponentUUID"))
                Actor.RootComponentUUID = static_cast<uint32>(ActorJson.at("RootComponentUUID").ToInt());

            Data.Actors.push_back(Actor);
        }
    }

    // Components
    if (Json.hasKey("Components"))
    {
        const JSON& CompsJson = Json.at("Components");
        for (size_t i = 0; i < CompsJson.size(); ++i)
        {
            const JSON& CompJson = CompsJson.at(i);
            FComponentData Comp;

            if (CompJson.hasKey("UUID"))
                Comp.UUID = static_cast<uint32>(CompJson.at("UUID").ToInt());
            if (CompJson.hasKey("OwnerActorUUID"))
                Comp.OwnerActorUUID = static_cast<uint32>(CompJson.at("OwnerActorUUID").ToInt());
            if (CompJson.hasKey("ParentComponentUUID"))
                Comp.ParentComponentUUID = static_cast<uint32>(CompJson.at("ParentComponentUUID").ToInt());
            if (CompJson.hasKey("Type"))
                Comp.Type = CompJson.at("Type").ToString();

            // Transform
            if (CompJson.hasKey("RelativeLocation"))
            {
                auto loc = CompJson.at("RelativeLocation");
                Comp.RelativeLocation = FVector(
                    (float)loc[0].ToFloat(),
                    (float)loc[1].ToFloat(),
                    (float)loc[2].ToFloat()
                );
            }

            if (CompJson.hasKey("RelativeRotation"))
            {
                auto rot = CompJson.at("RelativeRotation");
                Comp.RelativeRotation = FVector(
                    (float)rot[0].ToFloat(),
                    (float)rot[1].ToFloat(),
                    (float)rot[2].ToFloat()
                );
            }

            if (CompJson.hasKey("RelativeScale"))
            {
                auto scale = CompJson.at("RelativeScale");
                Comp.RelativeScale = FVector(
                    (float)scale[0].ToFloat(),
                    (float)scale[1].ToFloat(),
                    (float)scale[2].ToFloat()
                );
            }

            // Type별 속성
            if (CompJson.hasKey("StaticMesh"))
                Comp.StaticMesh = CompJson.at("StaticMesh").ToString();

            if (CompJson.hasKey("Materials"))
            {
                const JSON& matsJson = CompJson.at("Materials");
                for (size_t m = 0; m < matsJson.size(); ++m)
                {
                    Comp.Materials.push_back(matsJson.at(m).ToString());
                }
            }

            if (CompJson.hasKey("TexturePath"))
                Comp.TexturePath = CompJson.at("TexturePath").ToString();

            // 🔥 FireBallComponent (FFireBallProperty)
            if (Comp.Type.find("FireBallComponent") != std::string::npos &&
                CompJson.hasKey("FireBallData"))
            {
                const JSON& FireDataJson = CompJson.at("FireBallData");

                if (FireDataJson.hasKey("Intensity"))
                    Comp.FireBallProperty.Intensity = (float)FireDataJson.at("Intensity").ToFloat();

                if (FireDataJson.hasKey("Radius"))
                    Comp.FireBallProperty.Radius = (float)FireDataJson.at("Radius").ToFloat();

                if (FireDataJson.hasKey("RadiusFallOff"))
                    Comp.FireBallProperty.RadiusFallOff = (float)FireDataJson.at("RadiusFallOff").ToFloat();

                if (FireDataJson.hasKey("Color"))
                {
                    auto ColorJson = FireDataJson.at("Color");
                    if (ColorJson.size() >= 4)
                    {
                        Comp.FireBallProperty.Color = FLinearColor(
                            (float)ColorJson[0].ToFloat(),
                            (float)ColorJson[1].ToFloat(),
                            (float)ColorJson[2].ToFloat(),
                            (float)ColorJson[3].ToFloat()
                        );
                    }
                }
            }
            Data.Components.push_back(Comp);
        }
    }

    return Data;
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
