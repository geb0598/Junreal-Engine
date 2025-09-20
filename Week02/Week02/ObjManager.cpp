#include "pch.h"
#include "ObjManager.h"

#include "ObjectIterator.h"
#include "StaticMesh.h"
#include "Enums.h"
#include "ObjectFactory.h"

TMap<FString, FStaticMesh*> FObjManager::ObjStaticMeshMap;

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
    FWideString WPathFileName(PathFileName.begin(), PathFileName.end()); // 단순 ascii라고 가정

    FObjInfo RawObjInfo;
    FObjImporter::LoadObjModel(WPathFileName, &RawObjInfo, true, true);



    // 4) 맵에 추가 (Set이 아니라 Add)
    ObjStaticMeshMap.Add(PathFileName, NewFStaticMesh);

    // 5) 반환 경로 보장
    return NewFStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(FString& PathFileName)
{
    for (TObjectIterator<UStaticMesh> It; It; ++It)
    {
        UStaticMesh* StaticMesh = *It;
        if (StaticMesh->GetFilePath() == PathFileName)
        {
            return StaticMesh;
        }
    }

    FStaticMesh* StaticMeshAsset = FObjManager::LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = ObjectFactory::NewObject<UStaticMesh>();
    StaticMesh->SetStaticMeshAsset(StaticMeshAsset);
}
