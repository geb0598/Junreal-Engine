#pragma once
#include "Vector.h"
#include "Enums.h"

struct FVertexSimple
{
    FVector Position;
    FVector4 Color;

    void FillFrom(const FMeshData& mesh, size_t i)
    {
        Position = mesh.Vertices[i];
        Color = (i < mesh.Color.size()) ? mesh.Color[i] : FVector4(1, 1, 1, 1);
    }
};

struct FVertexDynamic
{
    FVector Position;
    FVector Normal;
    FVector4 Color;
    FVector2D UV;
    FVector4 Normal;

    void FillFrom(const FMeshData& mesh, size_t i)
 {
        Position = mesh.Vertices[i];
        Color = (i < mesh.Color.size()) ? mesh.Color[i] : FVector4(1, 1, 1, 1);
        UV = (i < mesh.UV.size()) ? mesh.UV[i] : FVector2D(0, 0);
        Normal = (i < mesh.Normal.size()) ? mesh.Normal[i] : FVector4(0, 0, 1, 0);
    }
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

    void FillFrom(const FMeshData& mesh, size_t i) {
        Position[0] = mesh.Vertices[i].X;
        Position[1] = mesh.Vertices[i].Y;
        Position[2] = mesh.Vertices[i].Z;
        CharSize[0] = (i < mesh.UV.size()) ? mesh.UV[i].X : 1.0f;
        CharSize[1] = (i < mesh.UV.size()) ? mesh.UV[i].Y : 1.0f;
        if (i < mesh.Color.size()) {
            UVRect[0] = mesh.Color[i].X;
            UVRect[1] = mesh.Color[i].Y;
            UVRect[2] = mesh.Color[i].Z;
            UVRect[3] = mesh.Color[i].W;
        }
        else {
            UVRect[0] = 0; UVRect[1] = 0; UVRect[2] = 1; UVRect[3] = 1;
        }
    }
};