#pragma once
#include "UEContainer.h"
#include "Vector.h"



// Raw Data
struct FObjInfo
{
    // Vertex List
    // UV List
    // Normal List
    // Vertex Index List
    // UV Index List
    // Normal Index List
    // Material List
    // Texture List
    // other...
};

struct FObjMaterialInfo
{
    // Diffuse Scalar
    // Diffuse Texture
    // other...
};

struct FObjImporter
{
    // Obj Parsing (*.obj to FObjInfo)
    // Material Parsing (*.obj to MaterialInfo)
    // Convert the Raw data to Cooked data (FStaticMesh)
    // other...
};

class UStaticMesh;

class FObjManager
{
private:
    static TMap<FString, FStaticMesh*> ObjStaticMeshMap;

public:
    static FStaticMesh* LoadObjStaticMeshAsset(const FString& PathFileName);

    static UStaticMesh* LoadObjStaticMesh(FString& PathFileName);
};