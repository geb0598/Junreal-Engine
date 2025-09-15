#pragma once
#include "Vector.h"

struct FVertexSimple
{
    FVector Position;
    FVector4 Color;
};

struct FVertexDynamic
{
    FVector Position;
    FVector4 Color;
    FVector2D UV;
    FVector4 Normal;
};

struct FBillboardVertexInfo {
    FVector WorldPosition;
    FVector2D CharSize;//char scale
    FVector4 UVRect;//uv start && uv size
};

struct FBillboardVertexInfo_GPU {
    float Position[3];
    float CharSize[2];
    float UVRect[4];
};


