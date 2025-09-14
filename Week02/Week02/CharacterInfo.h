#pragma once
#include "Vector.h"
struct FCharacterInfo
{
    FVector4 Info;//표준화된 uv좌표계, 표준화된 uv높이 너비
};

struct FBillboardCharInfo {
    FVector WorldPosition;
    FVector CharSize;
    FCharacterInfo UVRect;
};