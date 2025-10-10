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
    float4 DecalPosition = mul(float4(input.WorldPosition, 1.0f), DecalWorldMatrixInverse);
    
    float3 NDCPosition = mul(DecalPosition, DecalProjectionMatrix).xyz;
    
    if (abs(NDCPosition.x) > 1.0f ||
        abs(NDCPosition.y) > 1.0f ||
        abs(NDCPosition.z) > 1.0f)
    {
        discard;
    }
    float3 dpdx = ddx(input.WorldPosition);
  
    float3 dpdy = ddy(input.WorldPosition);

    float3 surfaceNormal = normalize(cross(dpdy, dpdx));
    
    // 데칼 방향(전방) - 데칼의 Z축 방향 (투영 방향)
    float3 decalForward = normalize(DecalWorldMatrixInverse._m10_m11_m12);

    // 표면 노멀과 데칼 투영 방향의 내적 계산
    // facing > 0: 앞면 (데칼 방향과 같은 방향)
    // facing < 0: 뒷면 (데칼 방향과 반대 방향)
    // facing ≈ 0: 수직면 (90도)
    float facing = dot(surfaceNormal, decalForward);

    // 앞면만 유지: facing이 0.17 이상이어야 함
    // 0.17 미만 = 수직면 + 뒷면 모두 제거
    // 값을 높이면 더 엄격하게 잘림 (0.5 = 60도)
    if (facing < 0.17f)
    {
        discard;
    }

    float2 DecalUV = float2(DecalPosition.x*0.5f + 0.5f, 1.0f - (DecalPosition.y*0.5f + 0.5f));

    float4 DecalColor = g_DecalTexture.Sample(g_Sample, DecalUV);
    return DecalColor;
}
