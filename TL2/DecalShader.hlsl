// DecalShader.hlsl - Screen-Space Decal Shader

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

cbuffer InvWorldBuffer : register(b4)
{
    row_major float4x4 InvWorldMatrix;
    row_major float4x4 InvViewProjMatrix;
}

// 데칼 텍스처
Texture2D g_DecalTexture : register(t0);
// Depth 버퍼 (화면 공간의 깊이 정보)
Texture2D g_DepthTexture : register(t1);

SamplerState g_Sample : register(s0);

struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL0;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD0;
};

struct PS_INPUT
{
    float4 position : SV_POSITION;
    float4 screenPos : TEXCOORD0;
    float4 worldPos : TEXCOORD1;
};

PS_INPUT mainVS(VS_INPUT input)
{
    PS_INPUT output;

    // 데칼 박스의 로컬 좌표를 월드 좌표로 변환
    float4 worldPos = mul(float4(input.position, 1.0f), WorldMatrix);
    output.worldPos = worldPos;

    // 월드 좌표를 화면 좌표로 변환
    float4x4 VP = mul(ViewMatrix, ProjectionMatrix);
    output.position = mul(worldPos, VP);

    // Screen space position for texture coordinate calculation
    output.screenPos = output.position;

    return output;
}

float4 mainPS(PS_INPUT input) : SV_TARGET
{
    // 1. 화면 좌표를 UV 좌표로 변환 (0~1 범위)
    float2 screenUV = input.screenPos.xy / input.screenPos.w;
    screenUV = screenUV * 0.5f + 0.5f;
    screenUV.y = 1.0f - screenUV.y; // Y축 반전

    // 2. Depth 버퍼에서 깊이 값 읽기
    float depth = g_DepthTexture.Sample(g_Sample, screenUV).r;

    // 3. 화면 좌표와 깊이를 이용해 월드 위치 복원
    // NDC 좌표 계산
    float4 ndcPos;
    ndcPos.xy = screenUV * 2.0f - 1.0f;
    ndcPos.y *= -1.0f;
    ndcPos.z = depth;
    ndcPos.w = 1.0f;

    // NDC에서 월드 공간 좌표로 복원
    float4 worldPos = mul(ndcPos, InvViewProjMatrix);
    worldPos /= worldPos.w; // perspective divide

    // 4. 월드 위치를 데칼 로컬 좌표로 변환
    float3 decalLocalPos = mul(worldPos, InvWorldMatrix).xyz;

    // 5. 데칼 박스 범위 체크 (-0.5 ~ 0.5)
    if (abs(decalLocalPos.x) > 0.5f ||
        abs(decalLocalPos.y) > 0.5f ||
        abs(decalLocalPos.z) > 0.5f)
    {
        discard; // 박스 밖이면 픽셀 버림
    }

    // 6. 로컬 좌표를 UV 좌표로 변환 (0~1)
    float2 decalUV = decalLocalPos.xy + 0.5f;

    // 7. 데칼 텍스처 샘플링
    float4 decalColor = g_DecalTexture.Sample(g_Sample, decalUV);

    // 8. 알파 값을 이용한 블렌딩
    // 투명도가 0이면 픽셀 버림
    if (decalColor.a < 0.01f)
    {
        discard;
    }

    // 9. 최종 색상 반환 (알파 블렌딩은 하드웨어에서 처리)
    return decalColor;
}
