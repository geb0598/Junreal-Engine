#pragma once
#include "Object.h"
#include"OBoundingBoxComponent.h"
#include"AABoundingBoxComponent.h"

struct FOctreeNode
{
    FBound Bounds;                       // 이 노드의 공간 범위
    TArray<AActor*> Actors;            // 이 범위 안의 Actor들
    FOctreeNode* Children[8] = { nullptr }; // 자식 노드 (최대 8개)
    
    UAABoundingBoxComponent* AABoundingBoxComponent;

    FOctreeNode(const FBound& InBounds)
        : Bounds(InBounds)
    {
        AABoundingBoxComponent = NewObject<UAABoundingBoxComponent>();
        AABoundingBoxComponent->SetMinMax(Bounds);
    }
};


class UOctree:public UObject
{
  
public:
    DECLARE_CLASS(UOctree, UObject)
    UOctree();
    ~UOctree();
    void Initialize(const FBound& InBounds);
    // 옥트리 빌드
    void Build(const TArray<AActor*>& InActors, FBound& WorldBounds, int32 Depth = 0);

    // 레이 기반 Actor 검색
    void Query(const FRay& Ray, TArray<AActor*>& OutActors) const;

    void Render(FOctreeNode* Root);
    // 전체 정리
    void Release();

private:
    FOctreeNode* Root = nullptr;
    int32 MaxDepth = 5;//최대 깊이 조절해봐야하고 
    // Leaf Node에 존재할 수 있는 최대 액터 수
    int32 MaxActorsPerNode = 5;//노드당 최소 엑터수 이거 이하여야만 끝나는 거 

    // 내부 함수들
    /**
    * @brief 자식 노드를 재귀적으로 생성합니다.
    * @param InActors 액터 배열
    * @param Bounds 해당 노드가 가진 범위
    * @param Depth 트리의 현재 깊이
    */
    FOctreeNode* BuildRecursive(const TArray<AActor*>& InActors, const FBound& Bounds, int32 Depth);
    void QueryRecursive(const FRay& Ray, FOctreeNode* Node, TArray<AActor*>& OutActors) const;
    void ReleaseRecursive(FOctreeNode* Node);
};