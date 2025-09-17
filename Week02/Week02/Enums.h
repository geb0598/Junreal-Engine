#ifndef UE_ENUMS_H
#define UE_ENUMS_H
#include "UEContainer.h"
#include "Enums.h"
#include <d3d11.h>   

struct FMeshData
{
	// 중복 없는 정점
	TArray<FVector> Vertices;//also can be billboard world position
	// 정점 인덱스
	TArray<uint32> Indices;
    // 중복 없는 정점
    TArray<FVector4> Color;//also can be UVRect
    // UV 좌표
    TArray<FVector2D> UV;//also can be Billboard size
    // 노말 좌표
    TArray<FVector4> Normal;
    //
    
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
    uint32 Stride = 0;
    uint32 Offset = 0;
    EPrimitiveTopology Topology = EPrimitiveTopology::TriangleList;
    D3D11_PRIMITIVE_TOPOLOGY Topol = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
};


struct FShader
{
    ID3D11InputLayout* SimpleInputLayout = nullptr;
    ID3D11VertexShader* SimpleVertexShader = nullptr;
    ID3D11PixelShader* SimplePixelShader = nullptr;
};

struct FTextureData
{
    ID3D11Resource* Texture = nullptr;
    ID3D11ShaderResourceView* TextureSRV = nullptr;
    ID3D11BlendState* BlendState = nullptr;
    ID3D11SamplerState* SamplerState = nullptr;
};

enum class EResourceType
{
    VertexBuffer,
    IndexBuffer
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

enum class ResourceDataType : uint8
{

};

enum class EVertexType : uint8
{
    None = -1,

    VertexSimple,
    VertexDynamic,

    End, 
};

//TODO EResourceType으로 재정의
enum class ResourceType : uint8
{
    None = -1,

    Mesh,
    DynamicMesh,
    Shader,
    Texture,
    Material,

    End
};

enum class EVertexLayoutType : uint8
{
    None,

    PositionColor,
    PositionColorTexturNormal,

    PositionBillBoard,
    PositionCollisionDebug,

    End,
};

enum class EViewModeIndex : uint32
{
    None,

    VMI_Lit,
    VMI_Unlit,
    VMI_Wireframe,

    End,
};

enum class EPrimitiveType : uint32
{
    None,

    Default,
    Sphere,

    End,
};

/**
 * Show Flag system for toggling rendering features globally
 * Uses bit flags for efficient storage and checking
 */
enum class EEngineShowFlags : uint64
{
    None = 0,
    
    // Primitive rendering
    SF_Primitives = 1ull << 0,    // Show/hide all primitive geometry
    SF_StaticMeshes = 1ull << 1,  // Show/hide static mesh actors
    SF_Wireframe = 1ull << 2,     // Show wireframe overlay
    
    // Debug features
    SF_BillboardText = 1ull << 3, // Show/hide UUID text above objects
    SF_BoundingBoxes = 1ull << 4, // Show/hide collision bounds
    SF_Grid = 1ull << 5,          // Show/hide world grid
    
    // Lighting
    SF_Lighting = 1ull << 6,      // Enable/disable lighting
    
    // Default enabled flags
    SF_DefaultEnabled = SF_Primitives | SF_StaticMeshes | SF_Grid,
    
    // All flags (for initialization/reset)
    SF_All = 0xFFFFFFFFFFFFFFFFull
};

// Bit flag operators for EEngineShowFlags
inline EEngineShowFlags operator|(EEngineShowFlags a, EEngineShowFlags b)
{
    return static_cast<EEngineShowFlags>(static_cast<uint64>(a) | static_cast<uint64>(b));
}

inline EEngineShowFlags operator&(EEngineShowFlags a, EEngineShowFlags b)
{
    return static_cast<EEngineShowFlags>(static_cast<uint64>(a) & static_cast<uint64>(b));
}

inline EEngineShowFlags operator~(EEngineShowFlags a)
{
    return static_cast<EEngineShowFlags>(~static_cast<uint64>(a));
}

inline EEngineShowFlags& operator|=(EEngineShowFlags& a, EEngineShowFlags b)
{
    a = a | b;
    return a;
}

inline EEngineShowFlags& operator&=(EEngineShowFlags& a, EEngineShowFlags b)
{
    a = a & b;
    return a;
}

// Helper function to check if a flag is set
inline bool HasShowFlag(EEngineShowFlags flags, EEngineShowFlags flag)
{
    return (flags & flag) != EEngineShowFlags::None;
}

#endif /** UE_ENUMS_H */
