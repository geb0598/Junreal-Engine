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

    // 데칼 로컬 공간에서 범위 체크 (타일링과 무관)
    if (abs(DecalPosition.x) > 0.5f ||
        abs(DecalPosition.y) > 0.5f ||
        abs(DecalPosition.z) > 0.5f)
    {
        discard;
    }

    // 타일링이 적용된 NDC 좌표 계산
    float3 NDCPosition = mul(DecalPosition, DecalProjectionMatrix).xyz;
    float3 dpdx = ddx(input.WorldPosition);
  
    float3 dpdy = ddy(input.WorldPosition);

    float3 surfaceNormal = normalize(cross(dpdy, dpdx));
    
    // 데칼 방향(전방) - 데칼의 X축 방향 (투영 방향)
    float3 decalForward = normalize(DecalWorldMatrixInverse._m10_m11_m12);

    // 표면 노멀과 데칼 투영 방향의 내적 계산
    // facing > 0: 앞면 (데칼 방향과 같은 방향)
    // facing < 0: 뒷면 (데칼 방향과 반대 방향)
    // facing ≈ 0: 수직면 (90도)
    float facing = dot(surfaceNormal, decalForward);//X축과 노멀값 내적 

    // 뒷면은 완전히 제거
    if (facing < 0.0f)
    {
        discard;
    }

    // 2. 각도 기반 페이드: 0.10 ~ 1.0 범위를 0 ~ 1 알파로 매핑
    // facing < 0.10: 거의 투명
    // facing = 0.5: 중간 투명도
    // facing = 1.0: 완전 불투명
    float angleFade = saturate((facing - 0.10f) / (1.0f - 0.10f));
    angleFade = pow(angleFade, 1.0f); // 비선형 감쇠로 더 자연스럽게

    // UV 계산 (DecalProjectionMatrix에 이미 타일링 스케일 적용됨)
    float2 DecalUV = float2(NDCPosition.x * 0.5f + 0.5f, 1.0f - (NDCPosition.y * 0.5f + 0.5f));

    // 4. 가장자리 페이드: frac를 사용하여 타일마다 페이드 적용
    float2 uvFrac = frac(DecalUV); // 0~1 범위로 반복
    float2 edgeDistance = abs(uvFrac - 0.5f) * 2.0f; // 0(중심) ~ 1(가장자리)
    float edgeFade = saturate(1.0f - max(edgeDistance.x, edgeDistance.y));
    edgeFade = pow(edgeFade, 1.0f); // 가장자리에서 부드럽게 페이드

    float4 DecalColor = g_DecalTexture.Sample(g_Sample, DecalUV);

    // 최종 알파 = 텍스처 알파 * 각도 페이드 * 가장자리 페이드
    DecalColor.a *= angleFade * edgeFade;

    return DecalColor;
}
