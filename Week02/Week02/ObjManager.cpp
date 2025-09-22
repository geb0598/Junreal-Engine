#include "pch.h"
#include "ObjManager.h"

#include "ObjectIterator.h"
#include "StaticMesh.h"
#include "Enums.h"

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

void FObjManager::Preload()
{
    LoadObjStaticMesh("Data/Cube.obj");
    LoadObjStaticMesh("Data/Sphere.obj");
    LoadObjStaticMesh("Data/Triangle.obj");
    LoadObjStaticMesh("Data/Arrow.obj");
    LoadObjStaticMesh("Data/RotationHandle.obj");
    LoadObjStaticMesh("Data/ScaleHandle.obj");
    LoadObjStaticMesh("Data/car.obj");
	LoadObjStaticMesh("Data/spaceCompound.obj");
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

    // TODO: 여기서 OBJ 파싱 및 NewFStaticMesh 초기화
    //FWideString WPathFileName(PathFileName.begin(), PathFileName.end()); // 단순 ascii라고 가정

    FObjInfo RawObjInfo;
    //FObjImporter::LoadObjModel(WPathFileName, &RawObjInfo, false, true); // test로 오른손 좌표계 false
    TArray<FObjMaterialInfo> MaterialInfos;
    FObjImporter::LoadObjModel(PathFileName, &RawObjInfo, MaterialInfos, true, true);
    FObjImporter::ConvertToStaticMesh(RawObjInfo, MaterialInfos, NewFStaticMesh);

    // 4) 맵에 추가 (Set이 아니라 Add)
    ObjStaticMeshMap.Add(PathFileName, NewFStaticMesh);

    // 5) 반환 경로 보장
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
    UStaticMesh* StaticMesh = UResourceManager::GetInstance().Load<UStaticMesh>(PathFileName);

	return StaticMesh;
}
