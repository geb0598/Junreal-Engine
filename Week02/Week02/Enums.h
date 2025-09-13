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

enum class EKeyInput : uint8
{
    // Keyboard Keys
    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
    Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9,
    Space, Enter, Escape, Tab, Shift, Ctrl, Alt,
    Up, Down, Left, Right,
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
    
    // Mouse Buttons
    LeftMouse, RightMouse, MiddleMouse, Mouse4, Mouse5,
    
    // Special
    Unknown
};

//TODO EResourceType으로 재정의
enum class ResourceType : uint8
{
    None = -1,

    Mesh,
    Shader,
    Texture,

    End
};

#endif /** UE_ENUMS_H */
