﻿#pragma once
#include "Vector.h"
#include "Enums.h"

struct FVertexUV
{
    FVector Position;
    FVector2D UV;
};

struct FVertexSimple
{
    FVector Position;
    FVector4 Color;

    void FillFrom(const FMeshData& mesh, size_t i)
    {
        Position = mesh.Vertices[i];
        Color = (i < mesh.Color.size()) ? mesh.Color[i] : FVector4(1, 1, 1, 1);
    }

    void FillFrom(const FNormalVertex& src) {
        Position = src.pos;
        Color = src.color;
    }
};

struct FVertexDynamic
{
    FVector Position;
    FVector Normal;
    FVector4 Color;
    FVector2D UV;
    FVector4 Tangent;

    void FillFrom(const FMeshData& mesh, size_t i)
 {
        Position = mesh.Vertices[i];
        Color = (i < mesh.Color.size()) ? mesh.Color[i] : FVector4(1, 1, 1, 1);
        UV = (i < mesh.UV.size()) ? mesh.UV[i] : FVector2D(0, 0);
        Normal = (i < mesh.Normal.size()) ? mesh.Normal[i] : FVector(0, 0, 1);
    }

    void FillFrom(const FNormalVertex& src) {
        Position = src.pos;
        Color = src.color;
        UV = src.tex;
        Normal = FVector{ src.normal.X, src.normal.Y, src.normal.Z };
        Tangent = src.tangent;
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

    void FillFrom(const FNormalVertex& src) {
        Position[0] = src.pos.X;
        Position[1] = src.pos.Y;
        Position[2] = src.pos.Z;

        CharSize[0] = src.tex.X;
        CharSize[1] = src.tex.Y;

        UVRect[0] = src.color.X;
        UVRect[1] = src.color.Y;
        UVRect[2] = src.color.Z;
        UVRect[3] = src.color.W;
    }
};