#pragma once
#include "Vector.h"



#define CBUFFER_INFO(TYPENAME, SLOTNUM, SETVS, SETPS)\
constexpr uint32 TYPENAME##SlotNum = SLOTNUM;\
constexpr bool TYPENAME##SetVS = SETVS;\
constexpr bool TYPENAME##SetPS = SETPS;\

#define CBUFFER_TYPE_LIST(MACRO)\
MACRO(ModelBufferType)					\
				\

CBUFFER_INFO(ModelBufferType, 0, true, false)


//Create 
//Release
//매크로를 통해 List에 추가하는 형태로 제작
//List를 통해 D3D11RHI
//Update
//Set vs,ps,hs,ds,gs

struct ModelBufferType
{
    FMatrix Model;
    uint32 UUID = 0;
    FVector Padding;
};


struct FConstants
{
    FVector WorldPosition;
    float Scale;
};
// b0 in VS


// b0 in PS
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
};

// b0 in StaticMeshPS
struct FPixelConstBufferType
{
    FMaterialInPs Material;
    bool bHasMaterial; // 1 bytes
    bool Dummy[3]; // 3 bytes padding
    bool bHasTexture; // 1 bytes
    bool Dummy2[11]; // 11 bytes padding
};

static_assert(sizeof(FPixelConstBufferType) % 16 == 0, "PixelConstData size mismatch!");

// b1 : ViewProjBuffer
struct ViewProjBufferType
{
    FMatrix View;
    FMatrix Proj;
};

// b2 : HighLightBuffer  (← 기존 코드에서 vpDesc를 다시 써서 버그났던 부분)
struct HighLightBufferType
{
    uint32 Picked;
    FVector Color;
    uint32 X;
    uint32 Y;
    uint32 Z;
    uint32 Gizmo;
};

struct ColorBufferType
{
    FVector4 Color;
};

// b5: Viewport 정보 (데칼용 Screen-Space UV 계산)
struct ViewportBufferType
{
    FVector4 ViewportRect; // x=StartX, y=StartY, z=SizeX, w=SizeY
};

struct DecalAlphaBufferType
{
    float CurrentAlpha;
    float pad[3];
};

struct BillboardBufferType
{
    FVector pos;
    FMatrix View;
    FMatrix Proj;
    FMatrix InverseViewMat;
    /*FVector cameraRight;
    FVector cameraUp;*/
};

//PS 5
struct UVScrollCB
{
    FVector2D Speed;
    float Time;
    float Pad;
};

//b4
struct DecalMatrix
{
    FMatrix DecalWorldMatrix;
    FMatrix DecalWorldMatrixInv;
    FMatrix DecalProjectionMatrix;
};