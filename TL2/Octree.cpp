#include "pch.h"
#include "Octree.h"

UOctree::UOctree()
{
    //Root =  NewObject<FOctreeNode>();
}

UOctree::~UOctree()
{
    Release();
}

void UOctree::Initialize(const FBox& InBounds)
{
    Root = new FOctreeNode(InBounds);
}

void UOctree::Build(const TArray<AActor*>& InActors, int32 Depth)
{
    if (Root)
    {
        ReleaseRecursive(Root);
        Root = nullptr;
    }

    // 월드 전체 Bounds는 외부에서 넘겨준다고 가정
    //Root = BuildRecursive(InActors, InActors.Num() > 0 ? InActors[0]->GetComponentsBoundingBox() : FBox(EForceInit::ForceInit), 0);
}

FOctreeNode* UOctree::BuildRecursive(const TArray<AActor*>& InActors, const FBox& Bounds, int32 Depth)
{
    if (InActors.Num() == 0) return nullptr;

    FOctreeNode* Node = new FOctreeNode(Bounds);

    // 이 노드 안에 들어가는 Actor 모으기
    for (AActor* Actor : InActors)
    {
      /*  if (Bounds.IsInsideOrOn(Actor->GetActorLocation()))
        {
            Node->Actors.Add(Actor);
        }*/
    }

    // 종료 조건
    if (Depth >= MaxDepth || Node->Actors.Num() <= MinActorsPerNode)
    {
        return Node;
    }

    // 8개 자식 영역으로 분할
   /* FVector Center = Bounds.GetCenter();
    FVector Extent = Bounds.GetExtent() * 0.5f;*/

    /*for (int i = 0; i < 8; i++)
    {
        FVector Offset(
            (i & 1 ? +1 : -1) * Extent.X,
            (i & 2 ? +1 : -1) * Extent.Y,
            (i & 4 ? +1 : -1) * Extent.Z
        );
        FBox ChildBounds(Center + Offset - Extent, Center + Offset + Extent);
        Node->Children[i] = BuildRecursive(Node->Actors, ChildBounds, Depth + 1);
    }*/

    return Node;
}

void UOctree::Query(const FRay& Ray, TArray<AActor*>& OutActors) const
{
    QueryRecursive(Ray, Root, OutActors);
}
void UOctree::Render(FOctreeNode* ParentNode) {
    if (ParentNode) {
        ParentNode->AABoundingBoxComponent->Render(UWorld::GetInstance().GetRenderer(), FMatrix::Identity(), FMatrix::Identity());
    }
    else {
        Root->AABoundingBoxComponent->Render(UWorld::GetInstance().GetRenderer(), FMatrix::Identity(), FMatrix::Identity());
    }
    for (auto ChildNode : Root->Children) {
        if (ChildNode) {
            Render(ChildNode);
        }
    }
}
void UOctree::QueryRecursive(const FRay& Ray, FOctreeNode* Node, TArray<AActor*>& OutActors) const
{
    if (!Node) return;

    // 레이-박스 교차 검사
  /*  if (!Ray.Intersects(Node->Bounds))
        return;*/

    // 노드 안의 Actor 검사
    OutActors.Append(Node->Actors);

    // 자식들 탐색
    for (int i = 0; i < 8; i++)
    {
        if (Node->Children[i])
            QueryRecursive(Ray, Node->Children[i], OutActors);
    }
}

void UOctree::Release()
{
    ReleaseRecursive(Root);
    Root = nullptr;
}

void UOctree::ReleaseRecursive(FOctreeNode* Node)
{
    if (!Node) return;

    for (int i = 0; i < 8; i++)
    {
        if (Node->Children[i])
            ReleaseRecursive(Node->Children[i]);
    }
    delete Node;
}