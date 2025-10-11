﻿#include "BoundingVolume.h"

bool IntersectRayAABB(const FRay& Ray, const FAABB& AABB, float& OutDistance)
{
    FVector invDir = FVector(1.0f / Ray.Direction.X, 1.0f / Ray.Direction.Y, 1.0f / Ray.Direction.Z);

    FVector t1 = (AABB.Min - Ray.Origin) * invDir;
    FVector t2 = (AABB.Max - Ray.Origin) * invDir;

    FVector tMin = t1.ComponentMin(t2);
    FVector tMax = t1.ComponentMax(t2);

    float tNear = std::max({ tMin.X, tMin.Y, tMin.Z });
    float tFar = std::min({ tMax.X, tMax.Y, tMax.Z });

    if (tNear > tFar || tFar < 0.0f) return false;

    OutDistance = (tNear < 0.0f) ? 0.0f : tNear;
    return true;
}


bool IntersectOBBAABB(const FOBB& OBB, const FAABB& AABB)
{
    FVector SATAxises[15];
    SATAxises[0] = FVector(1, 0, 0);
    SATAxises[1] = FVector(0, 1, 0);
    SATAxises[2] = FVector(0, 0, 1);
    SATAxises[3] = OBB.Axis[0];
    SATAxises[4] = OBB.Axis[1];
    SATAxises[5] = OBB.Axis[2];

    for (int i = 0; i < 3; i++)
    {
        SATAxises[6 + i * 3] = FVector::Cross(OBB.Axis[i], FVector(1, 0, 0));
        SATAxises[6 + i * 3 + 1] = FVector::Cross(OBB.Axis[i], FVector(0, 1, 0));
        SATAxises[6 + i * 3 + 2] = FVector::Cross(OBB.Axis[i], FVector(0, 0, 1));
    }

    for (FVector& SATAxis : SATAxises)
    {
        FVector Normal = SATAxis.GetNormalized();
        if (Normal == FVector(0, 0, 0))
        {
            continue;
        }

        //AABB Min, Max 구하기
        //한쪽 끝점과 반대쪽 끝점
        FVector AABBOuter1 = FVector(SATAxis.X > 0 ? AABB.Min.X : AABB.Max.X, SATAxis.Y > 0 ? AABB.Min.Y : AABB.Max.Y, SATAxis.Z > 0 ? AABB.Min.Z : AABB.Max.Z);
        FVector AABBOuter2 = FVector(SATAxis.X > 0 ? AABB.Max.X : AABB.Min.X, SATAxis.Y > 0 ? AABB.Max.Y : AABB.Min.Y, SATAxis.Z > 0 ? AABB.Max.Z : AABB.Min.Z);
        float AABBDotValue1 = Normal.Dot(AABBOuter1);
        float AABBDotValue2 = Normal.Dot(AABBOuter2);
        float AABBMin = AABBDotValue1 < AABBDotValue2 ? AABBDotValue1 : AABBDotValue2;
        float AABBMax = AABBDotValue1 < AABBDotValue2 ? AABBDotValue2 : AABBDotValue1;

        //OBB Min, Max 구하기
        float OBBMin = 0, OBBMax = 0;
        FVector OBBVertex = OBB.GetVertex(0);
        float OBBVertexDotValue = Normal.Dot(OBBVertex);
        OBBMin = OBBVertexDotValue;
        OBBMax = OBBVertexDotValue;
        for (int i = 1; i < 8; i++)
        {
            OBBVertex = OBB.GetVertex(i);
            OBBVertexDotValue = Normal.Dot(OBBVertex);
            if (OBBMin > OBBVertexDotValue)
            {
                OBBMin = OBBVertexDotValue;
            }
            else if (OBBMax < OBBVertexDotValue)
            {
                OBBMax = OBBVertexDotValue;
            }
        }

        //A의 최대가 B의 최소보다 작거나, B의 최대가 A의 최소보다 작으면 겹치지 않음
        if (AABBMax < OBBMin || OBBMax < AABBMin)
        {
            //한 축이라도 겹치지 않으면 충돌 x
            return false;
        }
    }
    return true;
}


