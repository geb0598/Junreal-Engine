#include "pch.h"
#include "ObjManager.h"

#include "ObjectIterator.h"
#include "StaticMesh.h"
#include "Enums.h"
#include "WindowsBinReader.h"
#include "WindowsBinWriter.h"
#include <filesystem>

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

void FObjManager::Preload()
{
    LoadObjStaticMesh("Data/tree9/trees9.obj"); // 해당 obj는 정점 수가 너무 많아서(약, 20만개 이상) 수집하는데 오래 걸림
    LoadObjStaticMesh("Data/Cube.obj");
    LoadObjStaticMesh("Data/Sphere.obj");
    LoadObjStaticMesh("Data/Triangle.obj");
    LoadObjStaticMesh("Data/Arrow.obj");
    LoadObjStaticMesh("Data/RotationHandle.obj");
    LoadObjStaticMesh("Data/ScaleHandle.obj");
    //LoadObjStaticMesh("Data/car.obj");
    LoadObjStaticMesh("Data/cube-tex.obj");
    LoadObjStaticMesh("Data/spaceCompound.obj");
    LoadObjStaticMesh("Data/cube_tex_blender.obj");
    LoadObjStaticMesh("Data/pony-cartoon/Pony_cartoon.obj");
    LoadObjStaticMesh("Data/ship/ship.obj");
    LoadObjStaticMesh("Data/OtherTeam/cube-tex2.obj");
}

void FObjManager::Clear()
{
    for (auto& Pair : ObjStaticMeshMap)
    {
        delete Pair.second;
    }

    ObjStaticMeshMap.Empty();
}

FStaticMesh* FObjManager::LoadObjStaticMeshAsset(const FString& PathFileName)
{
    // 2) 캐시 히트 시 즉시 반환 (Find는 FStaticMesh** 반환)
    if (FStaticMesh** It = ObjStaticMeshMap.Find(PathFileName))
    {
        return *It;
    }

    // 3) 캐시 미스: 새로 생성
    FStaticMesh* NewFStaticMesh = new FStaticMesh();

    //FWideString WPathFileName(PathFileName.begin(), PathFileName.end()); // 단순 ascii라고 가정

    // 4) 해당 파일명 bin이 존재하는 지 확인
    // 존재하면 bin을 가져와서 FStaticMesh에 할당
    // 존재하지 않으면, 아래 과정 진행 후, bin으로 저장
    std::filesystem::path Path(PathFileName);
    if (Path.extension() != ".obj")
    {
        UE_LOG("this file is not obj!: %s", PathFileName);
        return nullptr;
    }

    TArray<FObjMaterialInfo> MaterialInfos;

    std::filesystem::path WithoutExtensionPath = Path;
    WithoutExtensionPath.replace_extension("");
    const FString StemPath = WithoutExtensionPath.string(); // 확장자를 제외한 경로
    const FString BinPathFileName = StemPath + ".bin";
    if (std::filesystem::exists(BinPathFileName))
    {
        // obj 정보 bin으로 가져오기
        FWindowsBinReader Reader(BinPathFileName);
        Reader << *NewFStaticMesh;
        Reader.Close();

        // MaterialInfo도 bin으로 가져오기
        FString MatBinPathFileName = StemPath + "Mat.bin";
        if (!std::filesystem::exists(MatBinPathFileName))
        {
            UE_LOG("\'%s\' does not exists!", MatBinPathFileName);
            assert(std::filesystem::exists(StemPath + "Mat.bin") && "material bin file dont exists!");

            // 존재하지 않으므로 obj(mtl 파싱 위해선 obj도 파싱 필요) 및 Mtl 파싱
            FObjInfo RawObjInfo;
            FObjImporter::LoadObjModel(PathFileName, &RawObjInfo, MaterialInfos, true, true);

            // MaterialInfos를 관련 파일명으로 bin 저장
            FWindowsBinWriter MatWriter(StemPath + "Mat.bin");
            Serialization::WriteArray<FObjMaterialInfo>(MatWriter, MaterialInfos);
            MatWriter.Close();
        }
        else
        {
            UE_LOG("bin file \'%s\', \'%s\' load completed", BinPathFileName, MatBinPathFileName);
            FWindowsBinReader MatReader(StemPath + "Mat.bin");
            Serialization::ReadArray<FObjMaterialInfo>(MatReader, MaterialInfos);
            MatReader.Close();
        }  
    }
    else
    {
        // obj 및 Mtl 파싱
        FObjInfo RawObjInfo;
        //FObjImporter::LoadObjModel(WPathFileName, &RawObjInfo, false, true); // test로 오른손 좌표계 false
        FObjImporter::LoadObjModel(PathFileName, &RawObjInfo, MaterialInfos, true, true);
        FObjImporter::ConvertToStaticMesh(RawObjInfo, MaterialInfos, NewFStaticMesh);

        // obj 정보 bin에 저장
        FWindowsBinWriter Writer(BinPathFileName);
        Writer << *NewFStaticMesh;
        Writer.Close();

        // MaterialInfos도 관련 파일명으로 bin 저장
        FWindowsBinWriter MatWriter(StemPath + "Mat.bin");
        Serialization::WriteArray<FObjMaterialInfo>(MatWriter, MaterialInfos);
        MatWriter.Close();
    }

    // 리소스 매니저에 Material 리소스 맵핑
    for (const FObjMaterialInfo& InMaterialInfo : MaterialInfos)
    {
        UMaterial* Material = NewObject<UMaterial>();
        Material->SetMaterialInfo(InMaterialInfo);

        UResourceManager::GetInstance().Add<UMaterial>(InMaterialInfo.MaterialName, Material);
    }

    // 5) 맵에 추가 (Set이 아니라 Add)
    ObjStaticMeshMap.Add(PathFileName, NewFStaticMesh);

    // 6) 반환 경로 보장
    return NewFStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(const FString& PathFileName)
{
	// 1) 이미 로드된 UStaticMesh가 있는지 전체 검색
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetFilePath() == PathFileName)
        {
            return StaticMesh;
        }
    }

	// 2) 없으면 새로 로드
    UStaticMesh* StaticMesh = UResourceManager::GetInstance().Load<UStaticMesh>(PathFileName, EVertexLayoutType::PositionColorTexturNormal);

    UE_LOG("UStaticMesh(filename: \'%s\') is successfully crated!", PathFileName);
	return StaticMesh;
}
