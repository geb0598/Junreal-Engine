//======================================================
// ForwardDecalShader.hlsl - Forward 렌더링으로 projection decal Shading
//======================================================

cbuffer ModelBuffer : register(b0)
{
    row_major float4x4 WorldMatrix;
}

cbuffer ViewProjBuffer : register(b1)
{
    row_major float4x4 ViewMatrix;
    row_major float4x4 ProjectionMatrix;
}

cbuffer ColorBuffer : register(b3)
{
    float4 LerpColor;
}

cbuffer DecalTransformBuffer : register(b4)
{
    row_major float4x4 DecalWorldMatrixInverse;
    row_major float4x4 DecalProjectionMatrix;
}

cbuffer ViewportBuffer : register(b6)
{
    float4 ViewportRect; // x=StartX, y=StartY, z=SizeX, w=SizeY
}

//------------------------------------------------------
// Resources
//------------------------------------------------------
Texture2D g_DecalTexture : register(t0);
SamplerState g_Sample : register(s0);

//------------------------------------------------------
// Vertex Shader
//------------------------------------------------------
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL0;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION; // 픽셀 셰이더에서 자동으로 화면 픽셀 좌표로 변환됨
    float3 WorldPosition : WORLDPOSITION;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    // 월드 → 뷰 → 프로젝션 (row_major 기준)
    float4 WorldPosition = mul(float4(input.position, 1.0f), WorldMatrix);
    
    float4 ClipPosition = mul(mul(WorldPosition, ViewMatrix), ProjectionMatrix);
    output.position = ClipPosition;
    output.WorldPosition = WorldPosition.xyz;
    return output;
}

//------------------------------------------------------
// Pixel Shader
//------------------------------------------------------
float4 mainPS(PS_INPUT input) : SV_TARGET
{
    float3 DecalPosition = mul(float4(input.WorldPosition, 1.0f), DecalWorldMatrixInverse).xyz;
    
    //데칼 크기와 transform을 분리하고 프로젝션 구현이 완료되면 주석 해제
    //DecalPosition = mul(DecalPosition, DecalProjectionMatrix);
    if (abs(DecalPosition.x) > 1.0f ||
        abs(DecalPosition.y) > 1.0f ||
        abs(DecalPosition.z) > 1.0f)
    {
        discard;
    }
    
    float2 DecalUV = float2(DecalPosition.x + 0.5f, 1.0f - (DecalPosition.y + 0.5f));
    
    float4 DecalColor = g_DecalTexture.Sample(g_Sample, DecalUV);
    return DecalColor;
}
