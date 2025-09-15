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
