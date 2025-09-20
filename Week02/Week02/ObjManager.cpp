#include "pch.h"
#include "ObjManager.h"
#include "ObjectIterator.h"
#include "StaticMesh.h"
#include "Enums.h"

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

    // 4) 맵에 추가 (Set이 아니라 Add)
    ObjStaticMeshMap.Add(PathFileName, NewFStaticMesh);

    // 5) 반환 경로 보장
    return NewFStaticMesh;
}

UStaticMesh* FObjManager::LoadObjStaticMesh(FString& PathFileName)
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
    FStaticMesh* Asset = FObjManager::LoadObjStaticMeshAsset(PathFileName);
    UStaticMesh* StaticMesh = UResourceManager::GetInstance().Load<UStaticMesh>(PathFileName);
    
	// 3) Asset 연결
    StaticMesh->SetStaticMeshAsset(Asset);

	return StaticMesh;
}
