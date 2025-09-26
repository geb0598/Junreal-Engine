#pragma once
#include "Object.h"
#include"OBoundingBoxComponent.h"

struct FOctreeNode
{
    FBox Bounds;                       // 이 노드의 공간 범위
    TArray<AActor*> Actors;            // 이 범위 안의 Actor들
    FOctreeNode* Children[8] = { nullptr }; // 자식 노드 (최대 8개)

    FOctreeNode(const FBox& InBounds)
        : Bounds(InBounds)
    {
    }
};


class UOctree:public UObject
{
  
public:
    DECLARE_CLASS(UOctree, UObject)
    UOctree();
    ~UOctree();
    void Initialize(const FBox& InBounds);
    // 옥트리 빌드
    void Build(const TArray<AActor*>& InActors, int32 Depth = 0);

    // 레이 기반 Actor 검색
    void Query(const FRay& Ray, TArray<AActor*>& OutActors) const;

    // 전체 정리
    void Release();

private:
    FOctreeNode* Root = nullptr;
    int32 MaxDepth = 5;//최대 깊이 조절해봐야하고 
    int32 MinActorsPerNode = 5;//노드당 최소 엑터수 이거 이하여야만 끝나는 거 

    // 내부 함수들
    FOctreeNode* BuildRecursive(const TArray<AActor*>& InActors, const FBox& Bounds, int32 Depth);
    void QueryRecursive(const FRay& Ray, FOctreeNode* Node, TArray<AActor*>& OutActors) const;
    void ReleaseRecursive(FOctreeNode* Node);
};