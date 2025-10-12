#include "pch.h"
#include "BVH.h"
#include "StaticMeshActor.h"
#include "Picking.h"
#include "PickingTimer.h"
#include "UI/GlobalConsole.h"
#include <algorithm>
#include <cfloat>


FBVH::FBVH() : MaxDepth(0)
{
}

FBVH::~FBVH()
{
    Clear();
}

void FBVH::Build(const TArray<AActor*>& Actors)
{
    Clear();

    if (Actors.Num() == 0)
        return;

    //전체 액터 순회하며 StaticMeshComponent 추출
    TArray<UStaticMeshComponent*> StaticMeshComponents;
    for (const AActor* Actor : Actors)
    {
        if (!Actor || Actor->GetActorHiddenInGame())
        {
            continue;
        }

        for (auto Component : Actor->GetComponents())
        {
            if (UStaticMeshComponent* StaticMeshComp = Cast<UStaticMeshComponent>(Component))
            {
                StaticMeshComponents.Push(StaticMeshComp);
            }
        }
    }

    if (StaticMeshComponents.Num() == 0)
    {
        return;
    }
    Build(StaticMeshComponents);
   
}

void FBVH::Build(const TArray<UStaticMeshComponent*>& StaticMeshComps)
{
    Clear();
    TStatId BVHBuildStatId;
    FScopeCycleCounter BVHBuildTimer(BVHBuildStatId);
    // 1. 액터들의 AABB 정보 수집
    MeshBounds.Reserve(StaticMeshComps.Num());
    MeshIndices.Reserve(StaticMeshComps.Num());

    for (int i = 0; i < StaticMeshComps.Num(); ++i)
    {
        UStaticMeshComponent* StaticMeshComp = StaticMeshComps[i];
        const FAABB* MeshBounds_Local = nullptr;
        bool bHasBounds = false;

        MeshBounds.emplace_back(FBVHStaticMeshAABB(StaticMeshComp, StaticMeshComp->GetWorldAABB()));
        MeshIndices.Add(MeshBounds.Num() - 1);
    }

    // 2. 노드 배열 예약 (최악의 경우 2*N-1개 노드)
    Nodes.Reserve(MeshBounds.Num() * 2);

    // 3. 재귀적으로 BVH 구축
    MaxDepth = 0;
    int RootIndex = BuildRecursive(0, MeshBounds.Num(), 0);

    uint64_t BuildCycles = BVHBuildTimer.Finish();
    double BuildTimeMs = FPlatformTime::ToMilliseconds(BuildCycles);

    char buf[256];
    sprintf_s(buf, "[BVH] Built for %d actors, %d nodes, depth %d (Time: %.3fms)\n",
        MeshBounds.Num(), Nodes.Num(), MaxDepth, BuildTimeMs);
    UE_LOG(buf);
}


void FBVH::Clear()
{
    Nodes.Empty();
    MeshBounds.Empty();
    MeshIndices.Empty();
    MaxDepth = 0;
}

TArray<FVector> FBVH::GetBVHBoundsWire()
{
    TArray<FVector> BoundsWire;
    for (FBVHNode& Node : Nodes)
    {
        BoundsWire.Append(Node.BoundingBox.GetWireLine());
    }
    return BoundsWire;
}

UStaticMeshComponent* FBVH::Intersect(const FVector& RayOrigin, const FVector& RayDirection, float& OutDistance) const
{
    if (Nodes.Num() == 0)
        return nullptr;

    TStatId BVHIntersectStatId;
    FScopeCycleCounter BVHIntersectTimer(BVHIntersectStatId);

    OutDistance = FLT_MAX;
    UStaticMeshComponent* HitStaticMeshComponent = nullptr;

    // 최적화된 Ray 생성 (InverseDirection과 Sign 미리 계산)
    FOptimizedRay OptRay(RayOrigin, RayDirection);
    bool bHit = IntersectNode(0, OptRay, OutDistance, HitStaticMeshComponent);

    uint64_t IntersectCycles = BVHIntersectTimer.Finish();
    double IntersectTimeMs = FPlatformTime::ToMilliseconds(IntersectCycles);

    if (bHit)
    {
        char buf[256];
        sprintf_s(buf, "[BVH Pick] Hit actor at distance %.3f (Time: %.3fms)\n",
            OutDistance, IntersectTimeMs);
        UE_LOG(buf);
        return HitStaticMeshComponent;
    }
    else
    {
        char buf[256];
        sprintf_s(buf, "[BVH Pick] No hit (Time: %.3fms)\n", IntersectTimeMs);
        UE_LOG(buf);
        return nullptr;
    }
}

int FBVH::BuildRecursive(int FirstMeshBound, int MeshBoundCount, int Depth)
{
    MaxDepth = FMath::Max(MaxDepth, Depth);

    int NodeIndex = Nodes.Num();
    FBVHNode NewNode;
    Nodes.Add(NewNode);
    FBVHNode& Node = Nodes[NodeIndex];

    Node.BoundingBox = CalculateBounds(FirstMeshBound, MeshBoundCount);

    if (MeshBoundCount <= MaxActorsPerLeaf || Depth >= MaxBVHDepth)
    {
        Node.FirstMeshBound = FirstMeshBound;
        Node.MeshBoundCount = MeshBoundCount;
        return NodeIndex;
    }

    int BestAxis;
    float BestSplitPos;
    int SplitIndex = FindBestSplit(FirstMeshBound, MeshBoundCount, BestAxis, BestSplitPos);

    if (SplitIndex == FirstMeshBound || SplitIndex == FirstMeshBound + MeshBoundCount)
    {
        Node.FirstMeshBound = FirstMeshBound;
        Node.MeshBoundCount = MeshBoundCount;
        return NodeIndex;
    }

    int ActualSplit = PartitionActors(FirstMeshBound, MeshBoundCount, BestAxis, BestSplitPos);

    int LeftCount = ActualSplit - FirstMeshBound;
    int RightCount = MeshBoundCount - LeftCount;

    if (LeftCount == 0 || RightCount == 0)
    {
        Node.FirstMeshBound = FirstMeshBound;
        Node.MeshBoundCount = MeshBoundCount;
        return NodeIndex;
    }

    Node.LeftChild = BuildRecursive(FirstMeshBound, LeftCount, Depth + 1);
    Node.RightChild = BuildRecursive(ActualSplit, RightCount, Depth + 1);

    return NodeIndex;
}


FAABB FBVH::CalculateBounds(int FirstMeshBound, int MeshBoundCount) const
{
    FAABB Bounds;
    bool bFirst = true;

    for (int i = 0; i < MeshBoundCount; ++i)
    {
        int ActorIndex = MeshIndices[FirstMeshBound + i];
        const FAABB& ActorBound = MeshBounds[ActorIndex].AABB;

        if (bFirst)
        {
            Bounds = ActorBound;
            bFirst = false;
        }
        else
        {
            Bounds.Min.X = FMath::Min(Bounds.Min.X, ActorBound.Min.X);
            Bounds.Min.Y = FMath::Min(Bounds.Min.Y, ActorBound.Min.Y);
            Bounds.Min.Z = FMath::Min(Bounds.Min.Z, ActorBound.Min.Z);

            Bounds.Max.X = FMath::Max(Bounds.Max.X, ActorBound.Max.X);
            Bounds.Max.Y = FMath::Max(Bounds.Max.Y, ActorBound.Max.Y);
            Bounds.Max.Z = FMath::Max(Bounds.Max.Z, ActorBound.Max.Z);
        }
    }

    return Bounds;
}

FAABB FBVH::CalculateCentroidBounds(int FirstMeshBound, int MeshBoundCount) const
{
    FAABB Bounds;
    bool bFirst = true;

    for (int i = 0; i < MeshBoundCount; ++i)
    {
        int ActorIndex = MeshIndices[FirstMeshBound + i];
        const FVector& Center = MeshBounds[ActorIndex].Center;

        if (bFirst)
        {
            Bounds.Min = Bounds.Max = Center;
            bFirst = false;
        }
        else
        {
            Bounds.Min.X = FMath::Min(Bounds.Min.X, Center.X);
            Bounds.Min.Y = FMath::Min(Bounds.Min.Y, Center.Y);
            Bounds.Min.Z = FMath::Min(Bounds.Min.Z, Center.Z);

            Bounds.Max.X = FMath::Max(Bounds.Max.X, Center.X);
            Bounds.Max.Y = FMath::Max(Bounds.Max.Y, Center.Y);
            Bounds.Max.Z = FMath::Max(Bounds.Max.Z, Center.Z);
        }
    }

    return Bounds;
}

int FBVH::FindBestSplit(int FirstMeshBound, int MeshBoundCount, int& OutAxis, float& OutSplitPos)
{
    FAABB CentroidBounds = CalculateCentroidBounds(FirstMeshBound, MeshBoundCount);
    FAABB ParentBounds = CalculateBounds(FirstMeshBound, MeshBoundCount);

    FVector Extent = CentroidBounds.Max - CentroidBounds.Min;
    OutAxis = 0;
    if (Extent.Y > Extent.X) OutAxis = 1;
    if (Extent.Z > Extent[OutAxis]) OutAxis = 2;

    if (Extent[OutAxis] < KINDA_SMALL_NUMBER)
    {
        OutSplitPos = CentroidBounds.Min[OutAxis];
        return FirstMeshBound + MeshBoundCount / 2;
    }

    // 1) 정렬 - TArray의 Sort 사용
    // 임시 배열 생성 후 정렬
    TArray<int> TempIndices;
    TempIndices.Reserve(MeshBoundCount);
    for (int i = 0; i < MeshBoundCount; ++i)
    {
        TempIndices.Add(MeshIndices[FirstMeshBound + i]);
    }

    TempIndices.Sort([&](int A, int B)
    {
        return MeshBounds[A].Center[OutAxis] < MeshBounds[B].Center[OutAxis];
    });

    // 정렬된 결과를 다시 복사
    for (int i = 0; i < MeshBoundCount; ++i)
    {
        MeshIndices[FirstMeshBound + i] = TempIndices[i];
    }
    // 2) Prefix/Suffix AABB 계산
    TArray<FAABB> Prefix;
    TArray<FAABB> Suffix;
    Prefix.SetNum(MeshBoundCount);
    Suffix.SetNum(MeshBoundCount);

    Prefix[0] = MeshBounds[MeshIndices[FirstMeshBound]].AABB;
    for (int i = 1; i < MeshBoundCount; i++)
    {
        Prefix[i] = Prefix[i - 1] + MeshBounds[MeshIndices[FirstMeshBound + i]].AABB;
    }

    Suffix[MeshBoundCount - 1] = MeshBounds[MeshIndices[FirstMeshBound + MeshBoundCount - 1]].AABB;
    for (int i = MeshBoundCount - 2; i >= 0; i--)
    {
        Suffix[i] = Suffix[i + 1] + MeshBounds[MeshIndices[FirstMeshBound + i]].AABB;
    }

    // 3) SAH 비용 평가
    float BestCost = FLT_MAX;
    int BestSplit = FirstMeshBound + MeshBoundCount / 2;
    float SA_P = ParentBounds.GetSurfaceArea() + 1e-6f;

    for (int i = 0; i < MeshBoundCount - 1; i++)
    {
        int LeftCount = i + 1;
        int RightCount = MeshBoundCount - LeftCount;

        float SA_L = Prefix[i].GetSurfaceArea();
        float SA_R = Suffix[i + 1].GetSurfaceArea();

        float Cost = 1.0f + (SA_L / SA_P) * LeftCount + (SA_R / SA_P) * RightCount;

        if (Cost < BestCost)
        {
            BestCost = Cost;
            BestSplit = FirstMeshBound + LeftCount;
            OutSplitPos = MeshBounds[MeshIndices[BestSplit]].Center[OutAxis];
        }
    }

    return BestSplit;
}

//static inline float SurfaceArea(const FAABB& b) {
//    FVector s = b.Max - b.Min;
//    if (s.X <= 0 || s.Y <= 0 || s.Z <= 0) return 0.0f;
//    return 2.0f * (s.X * s.Y + s.Y * s.Z + s.Z * s.X);
//}

float FBVH::CalculateSAH(int FirstMeshBound, int LeftCount, int RightCount, const FAABB& Parent) const
{
    FAABB LB = CalculateBounds(FirstMeshBound, LeftCount);
    FAABB RB = CalculateBounds(FirstMeshBound + LeftCount, RightCount);

    float SA_P = Parent.GetSurfaceArea() + 1e-6f;
    float SA_L = LB.GetSurfaceArea();
    float SA_R = RB.GetSurfaceArea();

    constexpr float Ct = 1.0f;
    constexpr float Ci = 1.0f;
    return Ct + Ci * ((SA_L / SA_P) * LeftCount + (SA_R / SA_P) * RightCount);
}

int FBVH::PartitionActors(int FirstMeshBound, int MeshBoundCount, int Axis, float SplitPos)
{
    int Left = FirstMeshBound;
    int Right = FirstMeshBound + MeshBoundCount - 1;

    while (Left <= Right)
    {
        while (Left <= Right)
        {
            int LeftActorIndex = MeshIndices[Left];
            const FVector& LeftCenter = MeshBounds[LeftActorIndex].Center;
            if (LeftCenter[Axis] >= SplitPos)
                break;
            Left++;
        }

        while (Left <= Right)
        {
            int RightActorIndex = MeshIndices[Right];
            const FVector& RightCenter = MeshBounds[RightActorIndex].Center;
            if (RightCenter[Axis] < SplitPos)
                break;
            Right--;
        }

        if (Left < Right)
        {
            int Temp = MeshIndices[Left];
            MeshIndices[Left] = MeshIndices[Right];
            MeshIndices[Right] = Temp;
            Left++;
            Right--;
        }
    }

    return Left;
}

bool FBVH::IntersectNode(int NodeIndex,
    const FOptimizedRay& Ray,
    float& InOutDistance,
    UStaticMeshComponent*& OutStaticMeshComp) const
{
    const FBVHNode& Node = Nodes[NodeIndex];

    // 최적화된 Ray-AABB 교차 검사 사용
    float tNear;
    if (!IntersectOptRayAABB(Ray, Node.BoundingBox, tNear))
        return false;

    if (tNear >= InOutDistance)
        return false;

    if (Node.IsLeaf())
    {
        bool bHit = false;
        float Closest = InOutDistance;
        UStaticMeshComponent* ClosetMesh = nullptr;

        for (int i = 0; i < Node.MeshBoundCount; ++i)
        {
            int ActorIndex = MeshIndices[Node.FirstMeshBound + i];
            UStaticMeshComponent* StaticMeshComp = MeshBounds[ActorIndex].StaticMeshComp;

            float Dist;
            if (CPickingSystem::CheckStaticMeshPicking(StaticMeshComp, Ray, Dist))
            {
                if (Dist < Closest)
                {
                    Closest = Dist;
                    ClosetMesh = StaticMeshComp;
                    bHit = true;
                }
            }
        }

        if (bHit)
        {
            InOutDistance = Closest;
            OutStaticMeshComp = ClosetMesh;
        }

        return bHit;
    }

    struct ChildHit
    {
        int Index;
        float tNear;
        bool bValid;
    };

    auto TestChild = [&](int ChildIdx) -> ChildHit
        {
            if (ChildIdx < 0) return { ChildIdx, FLT_MAX, false };
            float tN;
            if (IntersectOptRayAABB(Ray,Nodes[ChildIdx].BoundingBox,tN))
                return { ChildIdx, tN, true };
            return { ChildIdx, FLT_MAX, false };
        };

    ChildHit L = TestChild(Node.LeftChild);
    ChildHit R = TestChild(Node.RightChild);

    bool bHit = false;

    if (L.bValid && R.bValid)
    {
        const ChildHit First = (L.tNear < R.tNear) ? L : R;
        const ChildHit Second = (L.tNear < R.tNear) ? R : L;

        if (IntersectNode(First.Index, Ray, InOutDistance, OutStaticMeshComp))
            bHit = true;

        if (InOutDistance > Second.tNear)
        {
            if (IntersectNode(Second.Index, Ray, InOutDistance, OutStaticMeshComp))
                bHit = true;
        }
    }
    else if (L.bValid)
    {
        if (IntersectNode(L.Index, Ray, InOutDistance, OutStaticMeshComp))
            bHit = true;
    }
    else if (R.bValid)
    {
        if (IntersectNode(R.Index, Ray, InOutDistance, OutStaticMeshComp))
            bHit = true;
    }

    return bHit;
}