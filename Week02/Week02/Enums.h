#ifndef UE_ENUMS_H
#define UE_ENUMS_H
#include "UEContainer.h"
#include "VertexSimple.h"
#include "Enums.h"
#include <d3d11.h>   

struct FMeshData
{
	// 중복 없는 정점
	TArray<FVertexSimple> Vertices;
	// 정점 인덱스
	TArray<uint32> Indices;
};
enum class EPrimitiveTopology
{
    PointList,
    LineList,
    LineStrip,
    TriangleList,
    TriangleStrip
};
struct FResourceData
{
    ID3D11Buffer* VertexBuffer = nullptr;
    ID3D11Buffer* IndexBuffer = nullptr;
    uint32 VertexCount = 0;     // 정점 개수
    uint32 IndexCount = 0;     // 버텍스 점의 개수 
    uint32 ByteWidth = 0;       // 전체 버텍스 데이터 크기 (sizeof(FVertexSimple) * VertexCount)
    EPrimitiveTopology Topology = EPrimitiveTopology::TriangleList;
};


struct FShader
{
    ID3D11InputLayout* SimpleInputLayout = nullptr;
    ID3D11VertexShader* SimpleVertexShader = nullptr;
    ID3D11PixelShader* SimplePixelShader = nullptr;
};

enum class EResourceType
{
    VertexBuffer,
    IndexBuffer
};

enum class EPrimitiveType 
{ 
    Cube, 
    Sphere, 
    Triangle,
    Arrow
};

enum class EGizmoMode : uint8
{
    Translate,
    Rotate,
    Scale
};
enum class EGizmoSpace : uint8
{
    World,
    Local
};
#endif /** UE_ENUMS_H */