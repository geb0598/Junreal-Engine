#pragma once
#include "Vector.h"

//CBufferStruct 안에 들어가는 Struct
struct FMaterialInPs
{
    FVector DiffuseColor; // Kd
    float OpticalDensity; // Ni

    FVector AmbientColor; // Ka
    float Transparency; // Tr Or d

    FVector SpecularColor; // Ks
    float SpecularExponent; // Ns

    FVector EmissiveColor; // Ke
    uint32 IlluminationModel; // illum. Default illumination model to Phong for non-Pbr materials

    FVector TransmissionFilter; // Tf
    float dummy; // 4 bytes padding

    FMaterialInPs() = default;
    FMaterialInPs(const FObjMaterialInfo& InMaterialInfo)
    {
        DiffuseColor = InMaterialInfo.DiffuseColor;
        AmbientColor = InMaterialInfo.AmbientColor;
    }
};




#define CBUFFER_INFO(TYPENAME, SLOTNUM, SETVS, SETPS)\
constexpr uint32 TYPENAME##SlotNum = SLOTNUM;\
constexpr bool TYPENAME##SetVS = SETVS;\
constexpr bool TYPENAME##SetPS = SETPS;\

#define CBUFFER_TYPE_LIST(MACRO)\
MACRO(ModelBufferType)					\
MACRO(ViewProjBufferType)					\
MACRO(BillboardBufferType)					\
MACRO(FPixelConstBufferType)					\
MACRO(HighLightBufferType)					\
MACRO(ColorBufferType)					\
MACRO(UVScrollCB)					\
MACRO(DecalMatrixCB)					\
MACRO(ViewportBufferType)					\
MACRO(DecalAlphaBufferType)					\
MACRO(HeightFogBufferType)                  \

CBUFFER_INFO(ModelBufferType, 0, true, false)
CBUFFER_INFO(ViewProjBufferType, 1, true, false)
CBUFFER_INFO(BillboardBufferType, 2, true, false)
CBUFFER_INFO(FPixelConstBufferType, 4, false, true)
CBUFFER_INFO(HighLightBufferType, 2, true, true)
CBUFFER_INFO(ColorBufferType, 3, false, true)
CBUFFER_INFO(UVScrollCB, 5, false, true)
CBUFFER_INFO(DecalMatrixCB, 7, false, true)
CBUFFER_INFO(ViewportBufferType, 6, false, true)
CBUFFER_INFO(DecalAlphaBufferType, 8, false, true)
CBUFFER_INFO(HeightFogBufferType, 8, false, true)


//Create 
//Release
//매크로를 통해 List에 추가하는 형태로 제작
//List를 통해 D3D11RHI
//Update
//Set vs,ps,hs,ds,gs

//VS : b0
struct ModelBufferType
{
    FMatrix Model;
    uint32 UUID = 0;
    FVector Padding;
};

//VS : b1
struct ViewProjBufferType
{
    FMatrix View;
    FMatrix Proj;
};

//VS : b2
struct BillboardBufferType
{
    FVector pos;
    float padding;
    FMatrix View;
    FMatrix Proj;
    FMatrix InverseViewMat;
    /*FVector cameraRight;
    FVector cameraUp;*/
};

//PS : b4
struct FPixelConstBufferType
{
    FMaterialInPs Material;
    uint32 bHasMaterial; // 4 bytes (HLSL bool is 4 bytes)
    uint32 bHasTexture;  // 4 bytes (HLSL bool is 4 bytes)
    float pad[2];        // 8 bytes padding for 16-byte alignment
};

//VS,PS : b2
struct HighLightBufferType
{
    uint32 Picked;
    FVector Color;
    uint32 X;
    uint32 Y;
    uint32 Z;
    uint32 Gizmo;
};

//PS : b3
struct ColorBufferType
{
    FVector4 Color;
};

//PS : b5
struct UVScrollCB
{
    FVector2D Speed;
    float Time;
    float Pad;
};

//PS : b7
struct DecalMatrixCB
{
    FMatrix DecalWorldMatrix;
    FMatrix DecalWorldMatrixInv;
    FMatrix DecalProjectionMatrix;
    FVector DecalSize;
    float Padding;
};

//PS : b6
struct ViewportBufferType
{
    FVector4 ViewportRect; // x=StartX, y=StartY, z=SizeX, w=SizeY
};

//PS : b8
struct DecalAlphaBufferType
{
    float CurrentAlpha;
    FVector2D UVTiling;
    float pad;
};

//PS : b8
struct HeightFogBufferType
{
    FLinearColor FogInscatteringColor;

    float FogDensity;
    float FogHeightFalloff;
    float StartDistance;
    float FogCutoffDistance;
    float FogMaxOpacity;
    float FogActorHeight;
    float Padding[2];
};

//---//



