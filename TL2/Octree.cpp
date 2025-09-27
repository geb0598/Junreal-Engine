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

void UOctree::Build(const TArray<AActor*>& InActors, const FBound& WorldBounds, int32 Depth)
{
    // 기존 트리가 있다면 정리
    Release();

    // 새로운 루트 노드 생성
    Initialize(WorldBounds);

    if (!Root || InActors.Num() == 0)
    {
        return;
    }

    BuildRecursive(Root, InActors, WorldBounds, Depth);
}

void UOctree::BuildRecursive(FOctreeNode* ChildNode, const TArray<AActor*>& InActors, const FBound& Bounds, int32 Depth)
{
    // 해당 Bound에 들어올 액터가 없으면 자식 노드 생성할 필요 없음
    if (InActors.Num() == 0) return;

    // 현재 노드 정보 설정
    ChildNode->Bounds = Bounds;
    ChildNode->Actors = InActors;

    // 리프 조건 (최대 깊이 or 최소 Actor 수)
    if (Depth >= MaxDepth || ChildNode->Actors.Num() <= MaxActorsPerNode)
    {
        // 리프 노드: 마이크로 BVH 생성 예약
        ChildNode->MarkDirty();
        return;
    }

    // 부모 노드 Bound 중심점 & 절반 크기
    FVector Center = Bounds.GetCenter();
    FVector HalfExtent = Bounds.GetExtent();
    FVector QuarterExtent = HalfExtent * 0.5f;

    // 8개 자식 Bound 생성
    FBound ChildBounds[8];
    for (int32 i = 0; i < 8; ++i)
    {
        FVector ChildCenter = Center;
        ChildCenter.X += (i & 1) ? QuarterExtent.X : -QuarterExtent.X;
        ChildCenter.Y += (i & 2) ? QuarterExtent.Y : -QuarterExtent.Y;
        ChildCenter.Z += (i & 4) ? QuarterExtent.Z : -QuarterExtent.Z;

        ChildBounds[i] = FBound(ChildCenter - QuarterExtent, ChildCenter + QuarterExtent);
    }

    // 자식별 Actor 분류
    TArray<AActor*> ChildActors[8];
    for (AActor* Actor : InActors)
    {
        FVector ActorLocation = Actor->GetActorLocation();

        // 각 자식 영역에 대해 검사하여 Actor가 포함되는지 확인
        for (int32 i = 0; i < 8; ++i)
        {
            if (ChildBounds[i].IsInside(ActorLocation))
            {
                ChildActors[i].Add(Actor);
                break; // 하나의 영역에만 속하므로 break
            }
        }
    }

    // 자식 노드 재귀 생성
    for (int32 i = 0; i < 8; ++i)
    {
        if (ChildActors[i].Num() > 0)
        {
            ChildNode->Children[i] = new FOctreeNode(ChildBounds[i]);
            BuildRecursive(ChildNode->Children[i], ChildActors[i], ChildBounds[i], Depth + 1);
        }
    }
    // 이 노드는 분할되었으므로 액터를 직접 소유하지 않음
    ChildNode->Actors.Empty();

    // 분할된 내부 노드는 마이크로 BVH 불필요
    if (ChildNode->MicroBVH)
    {
        delete ChildNode->MicroBVH;
        ChildNode->MicroBVH = nullptr;
    }
}
void UOctree::Render(FOctreeNode* ParentNode) {
    if (ParentNode) {
        ParentNode->AABoundingBoxComponent->Render(UWorld::GetInstance().GetRenderer(), FMatrix::Identity(), FMatrix::Identity());
    }
    else {
        if (!Root) {
            return;
        }
        Root->AABoundingBoxComponent->Render(UWorld::GetInstance().GetRenderer(), FMatrix::Identity(), FMatrix::Identity());
        ParentNode = Root;
    }
    for (auto ChildNode : ParentNode->Children) {
        if (ChildNode) {
            Render(ChildNode);
        }
    }
}
void UOctree::Query(const FRay& Ray, TArray<AActor*>& OutActors) const
{
    QueryRecursive(Ray, Root, OutActors);
}
// 수정 중
void UOctree::QueryRecursive(const FRay& Ray, FOctreeNode* Node, TArray<AActor*>& OutActors) const
{
    if (!Node) return;

    // 레이-박스 교차 검사
    float distance;
    if (!Node->Bounds.RayIntersects(Ray.Origin, Ray.Direction, distance))
    {
        return;
    }

    // Leaf Node일 경우 마이크로 BVH 사용
    if (Node->IsLeafNode())
    {
        if (Node->Actors.Num() > 0)
        {
            // Lazy rebuild 체크
            const_cast<FOctreeNode*>(Node)->EnsureMicroBVH();

            // 마이크로 BVH를 통한 정밀 검사
            if (Node->MicroBVH)
            {
                float hitDistance;
                AActor* HitActor = Node->MicroBVH->Intersect(Ray.Origin, Ray.Direction, hitDistance);
                if (HitActor)
                {
                    OutActors.Add(HitActor);
                }
            }
            else
            {
                // 백업: BVH가 없으면 기존 방식
                OutActors.Append(Node->Actors);
            }
        }
    }
    else // 중간 노드일 경우 근접순 재귀
    {
        // 자식 노드들을 거리순으로 정렬하여 처리
        struct FChildDistance
        {
            FOctreeNode* Child;
            float Distance;
        };

        TArray<FChildDistance> ChildDistances;
        for (int i = 0; i < 8; ++i)
        {
            if (Node->Children[i])
            {
                float childDistance;
                if (Node->Children[i]->Bounds.RayIntersects(Ray.Origin, Ray.Direction, childDistance))
                {
                    ChildDistances.Add({Node->Children[i], childDistance});
                }
            }
        }

        // 거리순 정렬 (가까운 순)
        ChildDistances.Sort([](const FChildDistance& A, const FChildDistance& B) {
            return A.Distance < B.Distance;
        });

        // 근접순으로 순회
        for (const FChildDistance& Child : ChildDistances)
        {
            QueryRecursive(Ray, Child.Child, OutActors);
        }
    }
}

void UOctree::Release()
{
    if (!Root)
    {
        return;
    }
    ReleaseRecursive(Root);
    Root = nullptr;
}

void UOctree::ReleaseRecursive(FOctreeNode* Node)
{
    if (!Node) return;

    for (int i = 0; i < 8; ++i)
    {
        if (Node->Children[i])
            ReleaseRecursive(Node->Children[i]);
    }
    if (Node->AABoundingBoxComponent)
    {
        ObjectFactory::DeleteObject(Node->AABoundingBoxComponent);
        Node->AABoundingBoxComponent = nullptr;
    }

    // 마이크로 BVH 정리 (Node의 소멸자에서도 처리되지만 명시적으로)
    if (Node->MicroBVH)
    {
        delete Node->MicroBVH;
        Node->MicroBVH = nullptr;
    }

    delete Node;
}