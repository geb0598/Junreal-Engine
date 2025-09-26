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

void UOctree::Initialize(const FBound& InBounds)
{
    Root = new FOctreeNode(InBounds);
}

void UOctree::Build(const TArray<AActor*>& InActors, FBound& WorldBounds, int32 Depth)
{
    // 기존 트리가 있다면 정리
    if (Root)
    {
        Release();
    }
    Root = BuildRecursive(InActors, WorldBounds, Depth); // Depth 기본값 0 

    // 월드 전체 Bounds는 외부에서 넘겨준다고 가정
    //Root = BuildRecursive(InActors, InActors.Num() > 0 ? InActors[0]->GetComponentsBoundingBox() : FBox(EForceInit::ForceInit), 0);
}

FOctreeNode* UOctree::BuildRecursive(const TArray<AActor*>& InActors, const FBound& Bounds, int32 Depth)
{
    // 해당 Bound에 들어올 액터가 없으면 자식 노드 생성할 필요 없음
    if (InActors.Num() == 0) return nullptr;

    FOctreeNode* Node = new FOctreeNode(Bounds);
    Node->Actors = InActors; // 일단 액터들을 현재 노드의 액터 배열에 할당

    // 분할 종료 조건(해당 노드가 Leaf) = 최대 깊이 도달 or 액터 수가 최대치 이하면 잎 노드 확정
    if (Depth >= MaxDepth || Node->Actors.Num() <= MaxActorsPerNode) return Node;

    // 분할 로직
    FVector Center = Node->Bounds.GetCenter();
    FVector HalfExtent = Node->Bounds.GetExtent() * 0.5f;

    FBound ChildBounds[8];
    for (int32 i = 0; i < 8;++i)
    {
        FVector ChildCenter = Center;
        ChildCenter.X += (i & 1 ? HalfExtent.X : -HalfExtent.X);
        ChildCenter.Y += (i & 2 ? HalfExtent.Y : -HalfExtent.Y);
        ChildCenter.Z += (i & 4 ? HalfExtent.Z : -HalfExtent.Z);
        ChildBounds[i] = FBound(ChildCenter - HalfExtent, ChildCenter + HalfExtent);
    }

    // 이 노드 안에 들어가는 Actor 모으기
    TArray<AActor*> ChildActors[8];
    for (AActor* Actor : Node->Actors)
    {
        FVector ActorLocation = Actor->GetActorLocation();
        for (int32 i = 0; i < 8; ++i)
        {
            // i번째 자식 공간에 있으면
            if (ChildBounds[i].IsInside(ActorLocation))
            {
                ChildActors[i].Add(Actor);
                break;
            }
        }
      /*  if (Bounds.IsInsideOrOn(Actor->GetActorLocation()))
        {
            Node->Actors.Add(Actor);
        }*/
    }

    // 분할 되었으므로, 현재 노드는 액터를 소유 X
    Node->Actors.Empty();

    for (int32 i = 0; i < 8; ++i)
    {
        if (ChildActors[i].Num() > 0)
        {
            // 노드 자식들에 대해 재귀적으로 생성
            Node->Children[i] = BuildRecursive(ChildActors[i], ChildBounds[i], Depth + 1);
        }
    }

    return Node;

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

    //return Node;
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